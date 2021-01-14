#include "Header.h"

using namespace std;

FileReader::FileReader(string FileName)
{
    this->inName = FileName;
}

void FileReader::doWork(optional<vector<string>>& Text)
{
    string TextBuf;

    ifstream in;

    if (Text != nullopt)
    {
        throw("mismatch of Input and Output");
    }

    vector<string> vec;
    Text = vec;

    in.open(this->inName);

    if (!in.is_open())
    {
        throw("Can not open " + this->inName);
    }

    while (!in.eof())
    {
        getline(in, TextBuf);

        Text->push_back(TextBuf);
    }

    in.close();
}

FileWriter::FileWriter(string OutputFileName)
{
    this->outName = OutputFileName;
}

void FileWriter::doWork(optional<vector<string>>& Text)
{
    if (Text == nullopt)
    {
        throw("mismatch of Input and Output");
    }

    string str;
    ofstream out;
    out.open(this->outName);

    if (!out.is_open())
    {
        throw("Can not open " + this->outName);
    }
    for (size_t i = 0; i < Text->size(); i++)
    {
        out << (*Text)[i];
        if (i + 1 < Text->size())
        {
            out << endl;
        }
    }

    out.close();
    Text = nullopt;
}

Grep::Grep(string Word)
{
    this->Word = Word;
}

void Grep::doWork(optional<vector<string>>& Text)
{
    if (Text == nullopt)
    {
        throw("mismatch of Input and Output");
    }
    for (size_t i = 0; i < Text->size(); i++)
    {
        if ((*Text)[i].find(this->Word) == string::npos)
        {
            Text->erase(Text->begin() + i);
        }
    }
}

void Sort::doWork(optional<vector<string>>& Text)
{
    if (Text == nullopt)
    {
        throw("mismatch of Input and Output");
    }
    sort(Text->begin(), Text->end());
}

Replace::Replace(string OldWord, string NewWord)
{
    this->OldWord = OldWord;
    this->NewWord = NewWord;
}

void Replace::doWork(optional<vector<string>>& Text)
{
    if (Text == nullopt)
    {
        throw("mismatch of Input and Output");
    }
    for (size_t i = 0; i < Text->size(); i++)
    {
        size_t index = (*Text)[i].find(OldWord);

        while (index != string::npos)
        {
            (*Text)[i].replace(index, OldWord.size(), NewWord);
            index = (*Text)[i].find(OldWord);
        }
    }
}

Dump::Dump(string OutputFileName)
{
    this->outName = OutputFileName;
}

void Dump::doWork(optional<vector<string>>& Text)
{
    if (Text == nullopt)
    {
        throw("mismatch of Input and Output");
    }
    string str;
    ofstream out;
    out.open(this->outName);
    if (!out.is_open())
    {
        throw("Can not open " + this->outName);
    }

    for (size_t i = 0; i < Text->size(); i++)
    {
        out << (*Text)[i] << endl;
    }

    out.close();
}

Parser::Parser(map<size_t, shared_ptr<Worker>> blocks, string InputFileName, string OutputFileName)
{
    this->blocks = blocks;
    this->inName = InputFileName;
    this->outName = OutputFileName;
}
shared_ptr<Worker> Parser::Parse(ifstream& Workflow)
{
    string BlockName;

    string FirstArgument;

    string SecondArgument;

    Workflow >> BlockName;

    if (BlockName != "=")
    {
        throw("Haven't '='");
    }

    Workflow >> BlockName;

    if (BlockName == "readfile")
    {
        string check;
        getline(Workflow, check);
        if (check.size() == 0)
        {
            throw("Not enough arguments in readfile description");
        }
        FirstArgument = check.substr(1, check.size());

        size_t SpaceIndex = 1;
        while (check[SpaceIndex] != ' ' && SpaceIndex < check.size())
        {
            SpaceIndex++;
        }
        if (SpaceIndex < check.size())
        {
            throw("Too much arguments in readfile description");
        }

        shared_ptr<Worker> Block = make_shared<FileReader>(FirstArgument);

        return Block;
    }
    else if (BlockName == "writefile")
    {
        string check;
        getline(Workflow, check);
        if (check.size() == 0)
        {
            throw("Not enough arguments in writefile description");
        }
        FirstArgument = check.substr(1, check.size());

        size_t SpaceIndex = 1;
        while (check[SpaceIndex] != ' ' && SpaceIndex < check.size())
        {
            SpaceIndex++;
        }
        if (SpaceIndex < check.size())
        {
            throw("Too much arguments in writefile description");
        }

        shared_ptr<Worker> Block = make_shared<FileWriter>(FirstArgument);

        return Block;
    }
    else if (BlockName == "grep")
    {
        string check;
        getline(Workflow, check);
        if (check.size() == 0)
        {
            throw("Not enough arguments in grep description");
        }
        FirstArgument = check.substr(1, check.size());

        size_t SpaceIndex = 1;
        while (check[SpaceIndex] != ' ' && SpaceIndex < check.size())
        {
            SpaceIndex++;
        }
        if (SpaceIndex < check.size())
        {
            throw("Too much arguments in grep description");
        }

        shared_ptr<Worker> Block = make_shared<Grep>(FirstArgument);

        return Block;
    }
    else if (BlockName == "sort")
    {
        string check;
        getline(Workflow, check);

        if (check.size() > 0)
        {
            throw("Too much arguments in sort description");
        }

        shared_ptr<Worker> Block = make_shared<Sort>();

        return Block;
    }
    else if (BlockName == "replace")
    {
        string check;
        getline(Workflow, check);
        if (check.size() == 0)
        {
            throw("Not enough arguments in replace description");
        }

        size_t SpaceIndex = 1;
        while (check[SpaceIndex] != ' ' && SpaceIndex < check.size())
        {
            SpaceIndex++;
        }
        if (SpaceIndex == check.size())
        {
            throw("Not enough arguments in replace description");
        }
        FirstArgument = check.substr(1, SpaceIndex - 1);
        SecondArgument = check.substr(SpaceIndex + 1, check.size()- SpaceIndex - 1);
        size_t SecondArgumentIndex = 0;
        while (SecondArgument[SecondArgumentIndex] != ' ' && SecondArgumentIndex < SecondArgument.size())
        {
            SecondArgumentIndex++;
        }
        if (SecondArgumentIndex < SecondArgument.size())
        {
            throw("Too much arguments in replace description");
        }

        shared_ptr<Worker> Block = make_shared<Replace>(FirstArgument, SecondArgument);

        return Block;
    }
    else if (BlockName == "dump")
    {
        string check;
        getline(Workflow, check);
        if (check.size() == 0)
        {
            throw("Not enough arguments in dump description");
        }
        FirstArgument = check.substr(1, check.size());

        size_t SpaceIndex = 1;
        while (check[SpaceIndex] != ' ' && SpaceIndex < check.size())
        {
            SpaceIndex++;
        }
        if (SpaceIndex < (check.size() - 1))
        {
            throw("Too much arguments in dump description");
        }

        shared_ptr<Worker> Block = make_shared<Dump>(FirstArgument);

        return Block;
    }
    else
    {
        throw("Bad name: " + BlockName);

        return nullptr;
    }
}

void WorkflowParser::Execute(string WorkflowFileName, string InputFileName, string OutputFileName, bool hasCommandLineInput,
    bool hasCommandLineOutput)
{
    size_t index;

    list<int> queue;

    map<size_t, shared_ptr<Worker>> blocks;

    ifstream Workflow;

    Workflow.open(WorkflowFileName);

    if (!Workflow.is_open())
    {
        throw("Can not open " + WorkflowFileName + " file");
    }

    string BlockName;

    Workflow >> BlockName;

    if (BlockName != "desc")
    {
        throw("Haven't 'desc' in" + WorkflowFileName);
    }

    if (Workflow.eof())
    {
        throw("EOF");
    }

    Workflow >> BlockName;

    while (BlockName != "csed")
    {
        if (Workflow.eof())
        {
            throw("You have to type a key word 'csed' and queue after the ids and blocks in" + WorkflowFileName);
        }

        index = stoi(BlockName);

        if (blocks.find(index) != blocks.end())
        {
            throw("id " + to_string(index) + " already used!");
        }

        blocks[index] = Parser::Parse(Workflow);
        Workflow >> BlockName;
    }

    if (Workflow.eof())
    {
        throw("You have to type a block queue after the 'csed' in the " + WorkflowFileName);
    }

    while (!Workflow.eof())
    {
        Workflow >> BlockName;

        index = stoi(BlockName);

        queue.push_back(index);

        if (Workflow.eof())
        {
            break;
        }

        string SpecialSymbol;
        Workflow >> SpecialSymbol;

        if (SpecialSymbol != "->")
        {
            throw("You have to type '->' between block's ids when you are making a queue");
        }

    }

    optional<vector<string>> Text;

    if (hasCommandLineInput)
    {
        FileReader object(InputFileName);
        object.doWork(Text);
    }

    for (auto i : queue)
    { 
        auto It = blocks.find(i);
        if (It == blocks.end())
        {
            string id = to_string(i);
            throw("id " + id + " doesn't exists in blocks description");
        }
        It->second->doWork(Text);
    }
    if (hasCommandLineOutput)
    {
        FileWriter object(OutputFileName);
        object.doWork(Text);
    }
}

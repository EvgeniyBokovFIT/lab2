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
    sort(Text->begin(), Text->end());
}

Replace::Replace(string OldWord, string NewWord)
{
    this->OldWord = OldWord;
    this->NewWord = NewWord;
}

void Replace::doWork(optional<vector<string>>& Text)
{
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
        Workflow >> FirstArgument;

        shared_ptr<Worker> Block = make_shared<FileReader>(FirstArgument);

        return Block;
    }
    else if (BlockName == "writefile")
    {
        Workflow >> FirstArgument;

        shared_ptr<Worker> Block = make_shared<FileWriter>(FirstArgument);

        return Block;
    }
    else if (BlockName == "grep")
    {
        Workflow >> FirstArgument;

        shared_ptr<Worker> Block = make_shared<Grep>(FirstArgument);

        return Block;
    }
    else if (BlockName == "sort")
    {
        shared_ptr<Worker> Block = make_shared<Sort>();

        return Block;
    }
    else if (BlockName == "replace")
    {
        Workflow >> FirstArgument;
        Workflow >> SecondArgument;

        shared_ptr<Worker> Block = make_shared<Replace>(FirstArgument, SecondArgument);

        return Block;
    }
    else if (BlockName == "dump")
    {
        Workflow >> FirstArgument;

        shared_ptr<Worker> Block = make_shared<Dump>(FirstArgument);;

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

    for (auto i = queue.begin(); i != queue.end(); )
    {
        blocks[*i]->doWork(Text);
        i++;
        if (!Text && i != queue.end())
        {
            blocks[*i]->doWork(Text);
            if (!Text)
            {
                throw("mismatch of Input and Output");
            }           
            i++;
        }       
    }   
    if (hasCommandLineOutput)
    {
        FileWriter object(OutputFileName);
        object.doWork(Text);
    }
}

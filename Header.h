#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include <optional>

using namespace std;

class Worker
{
public:
    virtual void doWork(optional<vector<string>>& Text) = 0;
};

class FileReader : public Worker
{
private:
    string inName;
public:
    FileReader(string FileName);

    void doWork(optional<vector<string>>& Text) override;
};

class FileWriter : public Worker
{
private:
    string outName;
public:
    FileWriter(string filename);

    void doWork(optional<vector<string>>& Text) override;
};

class Grep : public Worker
{
private:
    string Word;
public:
    Grep(string Word);

    void doWork(optional<vector<string>>& Text) override;
};

class Sort : public Worker
{
public:
    void doWork(optional<vector<string>>& Text) override;
};

class Replace : public Worker
{
private:
    string OldWord;

    string NewWord;
public:
    Replace(string OldWord, string NewWord);

    void doWork(optional<vector<string>>& Text) override;
};

class Dump : public Worker
{
private:
    string outName;
public:
    Dump(string OutputFileName);

    void doWork(optional<vector<string>>& Text) override;
};

class Parser
{
private:
    map<size_t, shared_ptr<Worker>> blocks;

    string inName;

    string outName;

public:
    Parser(map<size_t, shared_ptr<Worker>> blocks, string input, string output);

    static shared_ptr<Worker> Parse(ifstream& fin);
};

class WorkflowParser
{
public:
    static void Execute(string file, string input, string output, bool hasCommandLineInput, bool hasCommandLineOutput);
};
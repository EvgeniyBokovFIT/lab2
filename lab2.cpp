#include "Header.h"

using namespace std;
int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            throw exception("Too few arguments");
        }

        string WorkflowName = argv[1];

        string InputName;

        string OutputName;

        bool hasCommandLineInput = false;

        bool hasCommandLineOutput = false;

        if (argc > 2)
        {                                
            for (size_t i = 2; i < argc; i++)
            {
                
                if (argv[i] == (string)"-i")
                {
                    if (i + 1 < argc)
                    {
                        i++;
                        hasCommandLineInput = true;
                        InputName = argv[i];
                    }
                    i++;
                }
                if ((i < argc) && (argv[i] == (string)"-o"))
                {
                    if (i + 1 < argc)
                    {
                        i++;
                        hasCommandLineOutput = true;
                        OutputName = argv[i];
                    }
                }                
            }
            if ((!hasCommandLineInput))
            {
                if ((!hasCommandLineOutput))
                {
                    throw exception("Input and Output don't exist");
                }
                else
                {
                    throw exception("Input doesn't exist");
                }
            }
            if ((!hasCommandLineOutput))
            {
                throw exception("Output doesn't exist");
            }
        }

        WorkflowParser::Execute(WorkflowName, InputName, OutputName, hasCommandLineInput, hasCommandLineOutput);
    }
    catch (const char* ex)
    {
        cout << ex;
    }
    catch (string ex)
    {
        cout << ex;
    }
    catch (const exception& ex)
    {
        cout << ex.what();
    }

    return 0;
}

#include "alloc_block.h"
#include "max_block.h"
#include <list>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iterator>
#include <stdlib.h>
#include <sys/queue.h>
#include <unistd.h>
using namespace std;

// args options
enum alloc_strategies
{
    FF, //  first fit
    BF, //  best fit
    WF, //  worest fit
    INVALID
};

// retreiving args
alloc_strategies op_return(string str)
{
    if (str == "-ff")
        return FF;
    else if (str == "-bf")
        return BF;
    else if (str == "-wf")
        return WF;
    return INVALID;
}

// file reading
void readFile(vector<string> *arr, string filename)
{

    ofstream f;
    string line;

    int i = 0;
    // given file in canvas
    std::ifstream infile(filename);

    if (infile.is_open())
    {
        // loop through the file and get every line
        while (getline(infile, line))
        {
            std::istringstream iss(line);
            string word;
            // pushing the numbers to the above vars
            // as the arrangment explaind in the assignment specs
            if (!(iss >> word))
            {
                break;
            }
            arr->push_back(word);
            i++;
        }
    }
    f.close();
}

bool comp(const alloc_block *a, const alloc_block *b)
{
    return (void *)a->bword < (void *)b->bword;
}

void writeFile(int totalAllocationInByte, int totalAllocationTimes, list<alloc_block *> allocMBList, list<alloc_block *> freedMBList, string filename)
{

    std::ofstream f;
    f.open(filename);

    // file writing
    f << "***[Memory Allocated Size: " << totalAllocationInByte << " Bytes]***\n";
    f << "***[Memory Allocations: " << totalAllocationTimes << " Allocations]***\n";
    f << "***[Nodes in Free List: " << freedMBList.size() << " Nodes]***\n\n";

    // standerd output
    cout << "***[Memory Allocated Size: " << totalAllocationInByte << " Bytes]***\n";
    cout << "***[Memory Allocations: " << totalAllocationTimes << " Allocations]***\n";
    cout << "***[Nodes in Free List: " << freedMBList.size() << " Nodes]***\n\n";

    // file writing
    f << "freedMBList\n";
    f << "Address\t\t\tNode Size\n";

    // standerd output
    cout << "freedMBList\n";
    cout << "Address\t\t\tNode Size\n";
    for (list<alloc_block *>::iterator node = freedMBList.begin(); node != freedMBList.end(); ++node)
    {
        // file writing
        f << (void *)((*node)->bword) << "\t\t\t" << (*node)->bsize << "\n";
        // standerd output
        cout << (void *)((*node)->bword) << "\t\t\t" << (*node)->bsize << "\n";
    }

    // file writing
    f << "\n\n";
    f << "allocMBList\n";
    f << "Address\t\t\tNode Size\t\tContent\n";

    // standerd output
    cout << "\n\n";
    cout << "allocMBList\n";
    cout << "Address\t\t\tNode Size\t\tContent\n";

    for (list<alloc_block *>::iterator node = allocMBList.begin(); node != allocMBList.end(); ++node)
    {
        // file writing
        f << (void *)((*node)->bword) << "\t\t\t" << (*node)->bsize << "\t\t\t";
        // standerd output
        cout << (void *)((*node)->bword) << "\t\t\t" << (*node)->bsize << "\t\t\t";

        char buffer[(*node)->bsize + 1];
        snprintf(buffer, sizeof(buffer),
                 "%s", (*node)->bword);

        // file writing
        f << buffer << "\n";
        // standerd output
        cout << buffer << "\n";
    }

    f.close();
}

void allocateMemory(string &name, list<alloc_block *> *allocMBList, int *totalAllocationInByte, int *totalAllocationinTimes)
{
    // set the size
    size_t bsize = name.length();

    // allocate a new memory block with the name size
    void *bword;
    bword = sbrk(bsize);

    //copy from the name from the file to the specific memory block
    strncpy((char *)bword, name.c_str(), bsize);

    // push the newly allocated memory and size to the list
    alloc_block *mB = (alloc_block *)malloc(sizeof(struct alloc_block));
    mB->bsize = (int)bsize;
    mB->bword = (char *)bword;
    allocMBList->push_back(mB);

    // add size to totalAllocation for counting the bytes in the heap
    *totalAllocationInByte = *totalAllocationInByte + bsize;
    *totalAllocationinTimes = *totalAllocationinTimes + 1;
}

void compactMemory(list<alloc_block *> *freedMBList)
{
    // sort the list
    freedMBList->sort(comp);

    list<alloc_block *>::iterator it1 = freedMBList->begin(), it2 = ++freedMBList->begin();

    // iterate the sorted list
    while (it2 != freedMBList->end())
    {

        // checking consecitive free memory block
        if ((*it1)->bword + (*it1)->bsize == (*it2)->bword)
        {
            // merge blocks
            (*it1)->bsize = (*it1)->bsize + (*it2)->bsize;

            // erease the merged node
            freedMBList->erase(it2++);
        }
        else
            ++it1, ++it2;
    }
}

void deallocateMemory(int length, int maxLength, int counter, list<alloc_block *> *allocMBList, list<alloc_block *> *freedMBList)
{
    int remaining_length = length - counter;

    // length check for remaining names and should be limited to a max of maxLength names to be freed
    if (remaining_length > maxLength)
        remaining_length = maxLength;

    // free maxLength memory each iterationLength iteration
    for (int j = 0; j < remaining_length; j++)
    {

        // create iterator
        list<alloc_block *>::iterator it = allocMBList->begin();

        // random num
        std::srand(std::time(nullptr));
        int num = std::rand() % allocMBList->size();

        // advance iterator by num (get num-th node)
        advance(it, num);

        // add the removed to the freed list
        alloc_block *mB = (alloc_block *)malloc(sizeof(struct alloc_block));
        mB->bsize = (int)(*it)->bsize;
        mB->bword = (char *)(*it)->bword;
        freedMBList->push_back(mB);

        // remove from alloc list
        allocMBList->erase(it);
    }

    // compact consective memory in free list
    compactMemory(freedMBList);
}

void splitMemory(list<alloc_block *>::iterator it, string &name, list<alloc_block *> *allocMBList, list<alloc_block *> *freedMBList, bool *flagMB)
{
    // set the size
    size_t bsize = name.length();

    //copy from the name from the file to the specific memory block
    strncpy((char *)(*it)->bword, name.c_str(), bsize);

    // get the remaining size from that block
    int remainingSize = (*it)->bsize - (int)bsize;

    // store the new name in allocatMBList
    alloc_block *mB = (alloc_block *)malloc(sizeof(struct alloc_block));
    mB->bsize = (int)bsize;
    mB->bword = (char *)(*it)->bword;
    allocMBList->push_back(mB);

    // if remaining memory block store it back to freedMBList
    if (remainingSize > 0)
    {
        // char *word = static_cast<char *>(());
        alloc_block *mBF = (alloc_block *)malloc(sizeof(struct alloc_block));
        mBF->bsize = remainingSize;
        mBF->bword = (char *)(*it)->bword + bsize;
        freedMBList->push_back(mBF);
    }

    // delete the old memory block from the list
    freedMBList->erase(it);

    // compact consective memory in free list
    compactMemory(freedMBList);

    // set the flag so it's not allocated again in the next if block
    *flagMB = true;
}

void firstFit(string &name, list<alloc_block *> *allocMBList, list<alloc_block *> *freedMBList, bool *flagMB)
{
    // finding first fit
    for (list<alloc_block *>::iterator it = freedMBList->begin(); it != freedMBList->end(); ++it)
    {
        // checking the size of both the name from the file and the allocated memory size in freedMBList
        if (name.length() <= (unsigned)(*it)->bsize)
        {
            splitMemory(it, name, allocMBList, freedMBList, flagMB);
            break;
        }
    }
}

void worstFit(string &name, list<alloc_block *> *allocMBList, list<alloc_block *> *freedMBList, bool *flagMB)
{
    // max name size
    int max = 0;
    // temp iterator
    list<alloc_block *>::iterator temp;

    // iterating through freedMBList to get the max value and store that MB
    for (list<alloc_block *>::iterator it = freedMBList->begin(); it != freedMBList->end(); ++it)
    {
        if ((*it)->bsize > max)
        {
            max = (int)(*it)->bsize;
            temp = it;
        }
    }

    // checking the size of both the name from the file and the allocated memory size in freedMBList
    if (name.length() <= (unsigned)max && *temp != NULL)
    {
        splitMemory(temp, name, allocMBList, freedMBList, flagMB);
    }
}

void bestFit(string &name, list<alloc_block *> *allocMBList, list<alloc_block *> *freedMBList, bool *flagMB)
{
    // flag
    int i = -1;
    // temp iterator
    list<alloc_block *>::iterator temp;

    // iterating through freedMBList to get the exact match andd replace it with name
    for (list<alloc_block *>::iterator it = freedMBList->begin(); it != freedMBList->end(); ++it)
    {
        // checking the size of both the name from the file and the allocated memory size in freedMBList
        if (name.length() <= (unsigned)(*it)->bsize)
        {
            // first found satisfying size from list
            if (i == -1)
            {
                i = (*it)->bsize;
                temp = it;
            }
            // ideal size
            else if ((*temp)->bsize > (*it)->bsize)
            {
                i = (*it)->bsize;
                temp = it;
            }
        }
    }
    if (i != -1)
    {
        splitMemory(temp, name, allocMBList, freedMBList, flagMB);
    }
}

void run(vector<string> *arr, alloc_strategies strategy, string outFile)
{
    // lists
    list<alloc_block *> allocMBList, freedMBList;
    // length of file received
    int length = arr->size();
    // loop counter
    int i = 0;
    // free limit => 500
    int deletionLength = 500;
    // iteration length => 1000
    int insertionLength = 1000;
    // total allocated bytes
    int totalAllocatedMemoryInByte = 0, totalAllocatedMemoryTime = 0;

    // main loop for all name list size
    while (i < length)
    {
        // each 1000 allocations in allocMBList
        if (i % insertionLength == 0 && i > 0)
        {
            deallocateMemory(length, deletionLength, i, &allocMBList, &freedMBList);
        }

        // flag if memory block is found in freedMBList
        bool foundMB = false;

        // checking if not empty then we try use memory block
        if (freedMBList.size() > 0)
        {
            switch (strategy)
            {
            case FF:
                firstFit(arr->at(i), &allocMBList, &freedMBList, &foundMB);
                break;
            case BF:
                bestFit(arr->at(i), &allocMBList, &freedMBList, &foundMB);
                break;
            case WF:
                worstFit(arr->at(i), &allocMBList, &freedMBList, &foundMB);
                break;
            case INVALID:
                break;
            }
        }

        // if name didn't find a fit in freedMBList
        if (!foundMB)
        {
            allocateMemory(arr->at(i), &allocMBList, &totalAllocatedMemoryInByte, &totalAllocatedMemoryTime);
        }
        i++;
    }
    writeFile(totalAllocatedMemoryInByte, totalAllocatedMemoryTime, allocMBList, freedMBList, outFile);
}

int main(int argc, char *argv[])
{

    // vector for file reading and arguments
    vector<string> content;
    vector<string> arguments(argv + 1, argv + argc);

    // // help menu
    string help_menu = "Choose one of the following strategies as an argument:\n\t-ff \t<source file> <destination file>\tFirst-Fit\n\t-bf \t<source file> <destination file>\tBest-Fit\n\t-wf \t<source file> <destination file>\tWorst-Fit";

    // check if argument are given then enter the switch statement
    if (arguments.size() == 3)
    {
        readFile(&content, arguments[1]);

        switch (op_return(arguments[0]))
        {
        case FF:
            cout << "*********[First Fit]*********\n"
                 << endl;
            run(&content, FF, arguments[2]);
            break;
        case BF:
            cout << "*********[Best Fit]*********\n"
                 << endl;
            run(&content, BF, arguments[2]);
            break;
        case WF:
            cout << "*********[Worest Fit]*********\n"
                 << endl;
            run(&content, WF, arguments[2]);
            break;
        case INVALID:
            cout << help_menu << endl;
            break;
        default:
            break;
        }
    }
    else
    {
        cout << help_menu << endl;
    }

    return 0;
}

# Memory Allocation Strategies
### To compile the program
```bash
make
```
## app.cpp
* This file is the main engine for running the strategies.

## alloc_block.h
* This is the header file for the structure that represents each memory block in the heap.

## max_block.h
* This is the header file for the structure that represents highest/biggest memory block in the heap. 
* This header wasn't used but it's there for future enhancement on the time complexity for running worst fit.

## Requirements
- List file of strings on each line.

### To change deletion and insertion range
- Open `app.cpp` 
- Navigate to `run(...)`
- Change deletionLength var for the desired number of deletions after some node insertions.
- Change insertionLength var for the desired number of insertion  before deletions.

## For Help 
- pass -h argument as the box below show
    ```bash
    ./app -h
    ```

## Run First Fit allocation strategy
- pass -ff argument as the box below show
- `<source file>` is the input file
- `<destination file>` is the output file
    ```bash
    ./app -ff <source file> <destination file>
    ```

## Run Best Fit allocation strategy
- pass -bf argument as the box below show
- `<source file>` is the input file
- `<destination file>` is the output file
    ```bash
    ./app -bf <source file> <destination file>
    ```

## Run Worst Fit allocation strategy
- pass -wf argument as the box below show
- `<source file>` is the input file
- `<destination file>` is the output file
    ```bash
    ./app -wf <source file> <destination file>
    ```
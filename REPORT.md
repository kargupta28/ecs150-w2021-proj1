# Report for Project 1 ECS 150 sshell.c

### General 
The main goal for this project was to be able to build a working shell in c.
This project challenged us to learn more about how the UNIX environment works
in order to replicate a shell. The major learning curve was regarding syscalls
and pipelines.
Haree Akkala was in charge of doing Phase 1, Phase 2, Phase 3.
Karan Gupta was in charge of doing Phase 4, Phase 5, Phase 6.

### Testing
The testing of this project was done mostly on our computers while connected
to the CSIF environment. We tested each part / phase of the project separately.
The main issues regarding testing came when we had to test the pipeline phase.
The pipeline phased caused a lot of challenges especially with piping over
built-in functions. This took a few hours to resolve as we were unable to 
figure out why some of the outputs in pipeline phase had error vales greater 
than 1. Once the pipeline phase was figured out, we moved on to testing 
the special variables in the extra features phase. This also took a while to 
test as we enocountered multiple errors when trying to test with variables
that we specifically didn't assign any string value to in the commandline.

### Implementation
Most of the implementation was done from scratch with a couple of sources cited.
Forking was the first thing we had to look into to get started with this 
project. After that, we inplemented pharsing, pipelining, output redirection,
and the special feature.
Our high level design choice includes:
  1. Saving STDOut and STDIn at the start of main function, so we can reset it as we need.
  2. We faced quite a few errors trying to properly use pointers, etc, so for example we made extra "cmmd" for strtok to use, and we save "cmd" for printing at the end.
  3. We made 1 parse function which parses all the commands perfectly in 1 go. We did this so command execution is easier.
  4. To pipe between not just 3, but infinite pipes, we made our "executeCmds" function recursive. It can handle crazy requests like "pwd | pwd | pwd | pwd | pwd | pwd | echo Hello World | pwd | grep World | wc -l".

### Sources Cited
We used a sources, especially those provided in class, others include:
https://stackoverflow.com/questions/19099663/how-to-correctly-use-fork-exec-wait
https://www.tutorialspoint.com/unix_system_calls/waitpid.htm
https://likegeeks.com/shell-scripting-awesome-guide-part4/
https://cboard.cprogramming.com/c-programming/112536-implement-redirection-simple-shell.html
https://www.sarathlakshman.com/2012/09/24/implementation-overview-of-redirection-and-pipe-operators-in-shell
https://www.tutorialspoint.com/c_standard_library/c_function_freopen.htm
https://www.journaldev.com/40793/execvp-function-c-plus-plus
https://www.networkworld.com/article/3025497/sending-data-into-the-void-with-dev-null.html
https://stackoverflow.com/questions/21307013/recursive-piping-in-unix-again
https://stackoverflow.com/questions/20434124/recursive-piping-in-unix-environment
https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/

#### Notes:
We rushed the report a bit as we did not figure out phase 6 until really late.
We did not have enough time to work on the report, but I hope our code 
satisfies to your liking. 

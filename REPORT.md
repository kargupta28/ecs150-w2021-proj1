# Report for Project 1 ECS 150 sshell.c

### General 
The main goal for this project was to be able to build a working shell in c.
This project challenged us to learn more about how the UNIX environment works
in order to replicate a shell. The major learning curve was regarding syscalls
and pipelines.

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

### Sources Cited
https://javatutoring.com/c-program-remove-blank-spaces-from-string/
This one website helped us a lot especially in figuing out how to do the 
extra feature implementation as we had issues with blank variables being 
used for blank strings.


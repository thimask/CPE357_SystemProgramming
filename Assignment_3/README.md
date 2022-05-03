Write a program “monitor1” and “monitor2”.
Both fork a child process. They simulate a monitor situation. 

The child process should read a file name from the keyboard and print the files
information gathered with “stat”. If no file was found with that name, report this back
to the user and wait for the next user input.
Entering “list” lists the content of the current directory (wherever the executable is
being called in. Check: opendir, readdir and closedir).
Entering “q” ends the complete program (all processes) for monitor 1 and only this kid
process in monitor 2.


The parent process of monitor1:
The parent should check every 10 seconds, if the child was active. That means, if the user
entered a command. If the user has not entered a commend for 10 seconds, terminate the
child and then end the parent program. Prevent the parent process to be killed or terminated
from the command line. Right after the parent terminated the kid, make sure to wait or you
end up having a zombie.

The parent process of monitor2:
This monitor program makes sure, that the kid is always alive. The parent should wait(0) for the
child process to terminate. Here in monitor 2, “q” terminates the kid but NOT the parent, that’s
the difference. So if the kid is terminated, wait for 10 seconds (with sleep) and fork the child
again. This must be working indefinitelly. So how to end this for good? With kill the parent
from the command line (Its just a simulation of a monitor program!).

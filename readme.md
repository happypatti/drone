-----------------------------------------------------------------------------
Created by Ali Shalash on 6/10/2024. 
Started implementation around 8PM(EST) - Finished at around 11:30PM(EST)
-----------------------------------------------------------------------------

This is a simple client app that talks to the DRL server. It creates a message
with a name you provide, sends it to the server, gets a response, and then
prints that response. I used a state machine to handle the different stages
of the process. This setup is modular, so it's easy to tweak for different
kinds of messages and uses.

-----------------------------------------------------------------------------
To run:

1. Navigate to directory and type 'make'.
2. Once compiled, type './client'.
3. When prompted, enter First and Last name(no brackets necessary).
4. Receive response from server.


For Debug:

Modify: #define VERBOSE 0 to #define VERBOSE 1 to see output which helped debug my issues.

-----------------------------------------------------------------------------
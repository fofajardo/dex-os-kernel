/*
  Name: sync.c
  Copyright: 
  Author: Joseph Emmanuel DL Dayo
  Date: 18/01/04 06:27
  Description: Provides kernel synchornization functions
  -used hardware bit test and set instruction instead
*/



void sync_justwait(sync_sharedvar *var)
{
   while (var->busyflag && var->pid!=getprocessid());
};

void sync_entercrit(sync_sharedvar *var)
{
    while (testandset(&var->busyflag)) {
          if (var->pid==getprocessid()) break;
    }
    var->pid = getprocessid();
    var->wait++;
};


/*void sync_entercrit(sync_sharedvar *var)
{
    while (var->busy && var->pid!=getprocessid());
    var->busy = 1;
    var->pid = getprocessid();
    var->wait++;
};*/


void sync_leavecrit(sync_sharedvar *var)
{
 
    var->wait--;
    if (var->wait<0) {
         printf("sync: warning wrong number of enter-leave pairs detected!\n");
         printf("sync: sync value %d\n",var->wait);
         printf("sync: owner %s\n",var->owner);
     }
    if (var->wait==0) var->busyflag = 0;
};


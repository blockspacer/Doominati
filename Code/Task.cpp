//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM tasks.
//
//-----------------------------------------------------------------------------

#include "Code/Task.hpp"

#include "Code/Thread.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace Doom
{
   namespace Code
   {
      static Core::ListLink<Task> TaskFree;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
      //
      // Task constructor
      //
      Task::Task() :
         link{this},
         codePtr{nullptr},
         delay{0}
      {
         callStk.reserve(CallStkSize);
         dataStk.reserve(DataStkSize);
      }

      //
      // Task::stop
      //
      void Task::stop()
      {
         std::cerr << "Task::stop\n";
         for(auto &w : dataStk)
            std::cerr << "  " << w << '\n';

         // Release execution resources.
         callStk.clear();
         dataStk.clear();
         locReg.clear();

         // Move to free list.
         link.relink(&TaskFree);
      }

      //
      // Task::Create
      //
      Task *Task::Create(Thread *thrd)
      {
         Task *task;
         if(TaskFree.next->obj)
         {
            task = TaskFree.next->obj;
            task->link.unlink();
         }
         else
            task = new Task;

         task->prog = thrd->prog;
         task->proc = thrd->proc;
         task->thrd = thrd;

         task->link.insert(&thrd->tasks);

         task->delay = 0;

         return task;
      }
   }
}

// EOF

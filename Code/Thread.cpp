//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM threads.
//
//-----------------------------------------------------------------------------

#include "Code/Thread.hpp"

#include "Code/Function.hpp"
#include "Code/Process.hpp"
#include "Code/Task.hpp"

#include <algorithm>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // Thread constructor
   //
   Thread::Thread(Process *proc_) :
      link{this},
      proc{proc_},
      prog{proc_->prog}
   {
   }

   //
   // Thread destructor
   //
   Thread::~Thread()
   {
      while(tasks.next->obj)
         tasks.next->obj->stop();
   }

   //
   // Thread::exec
   //
   void Thread::exec()
   {
      for(auto itr = tasks.begin(), end = tasks.end(); itr != end;)
      {
         Task *task = &*itr++;

         if(task->state == TaskState::Exec)
            task->exec();

         if(task->state == TaskState::Stop && !task->join)
            task->stop();
      }
   }

   //
   // Thread::startTask
   //
   Task *Thread::startTask(Function *func, Word const *argV, Word argC)
   {
      Task *task = Task::Create(this);

      Word vaaC = argC > func->param ? argC - func->param : 0;
      auto vaaV = argV + argC - vaaC;

      // Copy variadic arguments.
      task->locReg.alloc(vaaC);
      std::copy(vaaV, vaaV + vaaC, &task->locReg[0]);

      // Copy normal arguments.
      task->locReg.alloc(std::max(func->local, func->param));
      std::copy(argV, vaaV, &task->locReg[0]);

      // Fill missing arguments.
      if(argC < func->param)
         std::fill(&task->locReg[argC], &task->locReg[func->param], 0);

      task->codePtr = func->entry;
      task->vaaRegC = vaaC;

      task->state = TaskState::Exec;

      return task;
   }
}

// EOF


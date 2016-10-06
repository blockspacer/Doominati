//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Memory access/control functions.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/Program.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   void NativeLoad_Memory() {} // See Code/Native.cpp.

   //
   // void *DGE_FreestoreBegin(void)
   //
   DGE_Code_NativeDefn(DGE_FreestoreBegin)
   {
      task->dataStk.push(task->prog->freestoreBegin);
      return false;
   }

   //
   // void *DGE_FreestoreEnd(void)
   //
   DGE_Code_NativeDefn(DGE_FreestoreEnd)
   {
      task->dataStk.push(task->prog->freestoreEnd);
      return false;
   }
}

// EOF


##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-code.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-code-lib
##
add_library(dge-code-lib ${DGE_SHARED_DECL}
   Callback.cpp
   Callback.hpp
   Codedefs.cpp
   Codedefs.hpp
   Convert.hpp
   ConvertStruct.hpp
   Defs.cpp
   Defs.hpp
   ExtMem.cpp
   ExtMem.hpp
   FuncPtr.hpp
   Function.hpp
   Glyph.cpp
   Glyph.hpp
   Memory.cpp
   Memory.hpp
   MemPtr.hpp
   MemStr.cpp
   MemStr.hpp
   Native.cpp
   Native.hpp
   Native/Debug.cpp
   Native/File.cpp
   Native/Memory.cpp
   Native/Print.cpp
   Native/String.cpp
   OpCode.cpp
   OpCode.hpp
   OpList.hpp
   Process.cpp
   Process.hpp
   Program.cpp
   Program.hpp
   Task.cpp
   TaskExec.cpp
   Task.hpp
   Textdefs.cpp
   Textdefs.hpp
   Thread.cpp
   Thread.hpp
   TuplePtr.hpp
   Types.hpp
)

target_link_libraries(dge-code-lib
   dge-core-lib
   dge-defs-lib
   dge-fs-lib
   ${GDCC_LIBRARIES})

DGE_INSTALL_PART(code FALSE TRUE)

## EOF


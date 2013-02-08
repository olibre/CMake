/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2013 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmTargetIncludeDirectoriesCommand.h"

#include "cmMakefileIncludeDirectoriesEntry.h"

//----------------------------------------------------------------------------
bool cmTargetIncludeDirectoriesCommand
::InitialPass(std::vector<std::string> const& args, cmExecutionStatus &)
{
  return this->HandleArguments(args, "INCLUDE_DIRECTORIES", PROCESS_BEFORE);
}

//----------------------------------------------------------------------------
void cmTargetIncludeDirectoriesCommand
::HandleImportedTarget(const std::string &tgt)
{
  cmOStringStream e;
  e << "Cannot specify include directories for imported target \""
    << tgt << "\".";
  this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
}

//----------------------------------------------------------------------------
void cmTargetIncludeDirectoriesCommand
::HandleMissingTarget(const std::string &name)
{
  cmOStringStream e;
  e << "Cannot specify include directories for target \"" << name << "\" "
       "which is not built by this project.";
  this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
}

//----------------------------------------------------------------------------
static bool isGeneratorExpression(const std::string &lib)
{
  const std::string::size_type openpos = lib.find("$<");
  return (openpos != std::string::npos)
      && (lib.find(">", openpos) != std::string::npos);
}

//----------------------------------------------------------------------------
std::string cmTargetIncludeDirectoriesCommand
::Join(const std::vector<std::string> &content)
{
  std::string dirs;
  std::string sep;
  std::string prefix = this->Makefile->GetStartDirectory() + std::string("/");
  for(std::vector<std::string>::const_iterator it = content.begin();
    it != content.end(); ++it)
    {
    if (cmSystemTools::FileIsFullPath(it->c_str())
        || isGeneratorExpression(*it))
      {
      dirs += sep + *it;
      }
    else
      {
      dirs += sep + prefix + *it;
      }
    sep = ";";
    }
  return dirs;
}

//----------------------------------------------------------------------------
void cmTargetIncludeDirectoriesCommand
::HandleDirectContent(cmTarget *tgt, const std::vector<std::string> &content,
                      bool prepend)
{
  cmListFileBacktrace lfbt;
  this->Makefile->GetBacktrace(lfbt);
  cmMakefileIncludeDirectoriesEntry entry(this->Join(content), lfbt);
  tgt->InsertInclude(entry, prepend);
}
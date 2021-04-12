// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $
//-- Constantine Kolokousis (kkolokou@ucsc.edu)
//-- Moises Perez(mperez86@ucsc.edu)

#include "commands.h"
#include "debug.h"
#include "file_sys.h"
#include <iomanip>

command_hash cmd_hash{
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
   {"#"     , fn_comm  },
};

command_fn find_command_fn(const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF('c', "[" << cmd << "]");
   const auto result = cmd_hash.find(cmd);
   if (result == cmd_hash.end()) {
      throw command_error(cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error(const string& what) :
   runtime_error(what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   if (words.size() > 1) {
      map <string, inode_ptr> path =
         state.get_cwd()->getCon()->getDirents();

      size_t i = 1;
      while (i < words.size()) {
         if (path.find(words[i]) == path.end()) {
            cout << "cat: " << words[i]
               << ": No such File or Directory" << endl;
            return;
         }
         if (!(path.find(words[i])->second->getCon()->File_Bool())) {
            cout << "cat: " << words[i]
               << ": Is a Directory" << endl;
            return;
         }
         i++;
      }
      i = 1;
      while (i < words.size()) {
         auto file = path.find(words[i]);
         cout << file->second->getCon()->readfile() << endl;
         i++;
      }
   }
   else if (words.size() == 1) {
      cout << "cat: need a filename" << endl;
      return;
   }
}

void fn_cd(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   if (words.size() == 1 || words[1] == "/" || words[1] == "..") {
      state.set_cwd(state.get_root());
      state.setCurrDirectory("/");
      return;
   }
   else {
      map <string, inode_ptr> path =
         state.get_cwd()->getCon()->getDirents();
      size_t i = 1;
      while(i < words.size()) {
         if (path.find(words[i]) == path.end()) {
            cout << "cd: " << words[i]
               << ": No such File or Directory" << endl;
            return;
         }
         if ((path.find(words[i])->second->getCon()->File_Bool())) {
            cout << "cd: " << words[i]
               << ": Is a File" << endl;
            return;
         }
         i++;
      }
      state.setCurrDirectory(state.getCurrDirectory()
         + words[1] + "/");
      state.set_cwd(path.find(words[1])->second);
   }
}

void fn_echo(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);
   cout << word_range(words.cbegin() + 1, words.cend()) << endl;
}

void fn_exit(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);
   throw ysh_exit();
}

void fn_ls(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   map <string, inode_ptr> path;
   if (words.size() == 1 || words[1] == "/" || words[1] == ".") {
      cout << state.getCurrDirectory() << ":" << endl;

      path = state.get_cwd()->getCon()->getDirents();
      auto i = path.begin();
      while (i != path.end()) {
         cout << setw(6) << i->second->get_inode_nr()
            << " ";
         cout << setw(6) << i->second->getCon()->size()
            << " ";
         cout << i->first;
         if (!(i->second->getCon()->File_Bool())) {
            cout << "/";
         }
         cout << endl;
         i++;
      }
   }
   else {
      if (words[1] == "..") {
         path = state.get_cwd()->getCon()->getDirents();
         inode_ptr reDir = state.get_cwd();
         string reName = state.getCurrDirectory();

         state.set_cwd(path.find("..")->second);
         wordvec newName = split(state.getCurrDirectory(), "/");

         string newDir = "/";
         size_t i = 0;
         while (i < newName.size() - 1) {
            newDir += newName[i] + "/";
            i++;
         }
         state.setCurrDirectory(newDir);
         cout << state.getCurrDirectory() << ":" << endl;

         path = state.get_cwd()->getCon()->getDirents();
         for (auto j = path.begin(); j != path.end(); j++) {
            cout << setw(6) << j->second->get_inode_nr()
               << " ";
            cout << setw(6) << j->second->getCon()->size()
               << " ";
            cout << j->first;
            if (!(j->second->getCon()->File_Bool())) {
               cout << "/";
            }
            cout << endl;
         }
         state.set_cwd(reDir);
         state.setCurrDirectory(reName);
      }
   }
}

void fn_lsr(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   map <string, inode_ptr> path =
      state.get_cwd()->getCon()->getDirents();
   
   inode_ptr temp = state.get_cwd();
   string reName = state.getCurrDirectory();

   fn_ls(state, words);

   for (auto i = path.begin(); i != path.end(); i++) {
      if ((i->second->getCon()->File_Bool() == false)
         and i->first != "." and i->first != "..") {
         state.get_cwd() = i->second;
         fn_lsr(state, words);
      }
   }
   state.getCurrDirectory() = reName;
   state.get_cwd() = temp;
}

void fn_make(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);
   if (words.size() == 1) {
      cout << "make: " << "need filename" << endl;
      return;
   }
   else {
      map <string, inode_ptr> path =
         state.get_cwd()->getCon()->getDirents();
      if (path.find(words[1]) != path.end()) {
         cout << "make: " << words[1]
            << ": File or Directory already exists" << endl;
         return;
      }
      string newFilename = words[1];
      string contents;
      size_t i = 2;
      while (i < words.size()) {
         contents += words[i] + " ";
         i++;
      }
      wordvec fwords = split(contents, " ");
      inode_ptr file = state.get_cwd()->getCon()->mkfile(newFilename);
      file->getCon()->writefile(fwords);
   }
}

void fn_mkdir(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   if (words.size() > 1) {
      map <string, inode_ptr> path =
         state.get_cwd()->getCon()->getDirents();
      if (path.find(words[1]) != path.end()) {
         cout << "mkdir: " << words[1]
            << ": File or directory already exists" << endl;
         return;
      }
      string newDirectory = words[1];
      inode_ptr Directory = state.get_cwd()->getCon()
         ->mkdir(newDirectory);
      Directory->getCon()->addBranch(".", Directory);
      Directory->getCon()->addBranch("..", state.get_cwd());
   }
   else if (words.size() == 1){
      cout << "mkdir: " << "need directory name" << endl;
      return;
   }
   else {
      cout << "mkdir: " << "need directory name" << endl;
      return;
   }
}

void fn_prompt(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   if (words.size() == 1) {
      cout << "prompt => need a prompt";
      return;
   }
   string newPrompt = "";
   for (size_t i = 1; i < words.size(); i++) {
      newPrompt += words[i] + " ";
   }
   state.set_prompt(newPrompt);
}

void fn_pwd(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   cout << state.getCurrDirectory() << endl;
}

void fn_rm(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);

   if (words.size() == 1) {
      cout << "rm: need File or Directory" << endl;
      return;
   }
   map<string, inode_ptr> path =
      state.get_cwd()->getCon()->getDirents();
   if (path.find(words[1]) == path.end()) {
      cout << "rm: " << words[1]
         << ": file or Directory does not exist" << endl;
      return;
   }
   else if (!(path.find(words[1])->second->getCon()->File_Bool())) {
      if (path.find(words[1])->second->getCon()->size() > 2) {
         cout << "rm: Cannot delete non-empty Directory" << endl;
      }
      else {
         state.get_cwd()->getCon()->remove(words[1]);
      }
   }
   else {
      state.get_cwd()->getCon()->remove(words[1]);
   }
}
//Didn't get to it 
void fn_rmr(inode_state& state, const wordvec& words) {
   DEBUGF('c', state);
   DEBUGF('c', words);
}

void fn_comm(inode_state&, const wordvec&) {
   return;
}

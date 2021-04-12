// $Id: file_sys.cpp,v 1.9 2020-10-26 21:32:08-07 - - $
//-- Constantine Kolokousis (kkolokou@ucsc.edu)
//-- Moises Perez(mperez86@ucsc.edu)

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "commands.h"
#include "file_sys.h"

size_t inode::next_inode_nr{ 1 };

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
   case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
   case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
   default: assert(false);
   };
   return out;
}

inode_state::inode_state() {
   DEBUGF('i', "root = " << root << ", cwd = " << cwd
      << ", prompt = \"" << prompt() << "\"");

   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->addBranch(".", root);
   currDirectory = "/";
   root->contents->addBranch("..", root);
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
      << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type) : inode_nr(next_inode_nr++) {
   switch (type) {
   case file_type::PLAIN_TYPE:
      contents = make_shared<plain_file>();
      contents->setFile(true);
      break;
   case file_type::DIRECTORY_TYPE:
      contents = make_shared<directory>();
      contents->setFile(false);
      break;
   default: assert(false);
   }
   DEBUGF('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF('i', "inode = " << inode_nr);
   return inode_nr;
}

file_error::file_error(const string& what) :
   runtime_error(what) {
}

const wordvec& base_file::readfile() const {
   throw file_error("is a " + error_file_type());
}

void base_file::writefile(const wordvec&) {
   throw file_error("is a " + error_file_type());
}

void base_file::remove(const string&) {
   throw file_error("is a " + error_file_type());
}

inode_ptr base_file::mkdir(const string&) {
   throw file_error("is a " + error_file_type());
}

inode_ptr base_file::mkfile(const string&) {
   throw file_error("is a " + error_file_type());
}

map<string, inode_ptr> base_file::getDirents() {
   throw file_error("is a " + error_file_type());
}

void directory::addBranch(const string& filename, inode_ptr pointer){
   dirents.insert(std::pair<string, inode_ptr>(filename, pointer));
}

void base_file::addBranch(const string& filename, inode_ptr pointer) {
   DEBUGF('i', filename);
   DEBUGF('i', pointer);
   throw file_error("is a " + error_file_type());
}

size_t plain_file::size() const {
   size_t size = 0;
   DEBUGF('i', "size = " << size);
   if (data.size() == 0) {
      return size;
   }
   else {
      size = data.size();
   }
   for (auto i : data) {
      size += i.size();
   }
   return size - 1;
}

const wordvec& plain_file::readfile() const {
   DEBUGF('i', data);
   return data;
}

void plain_file::writefile(const wordvec& words) {
   DEBUGF('i', words);
   data.clear();
   data = words;
}

size_t directory::size() const {
   size_t size = dirents.size();
   DEBUGF('i', "size = " << size);
   return size;
}

void directory::remove(const string& filename) {
   DEBUGF('i', filename);
   dirents.erase(filename);
}

inode_ptr directory::mkdir(const string& dirname) {
   DEBUGF('i', dirname);
   inode_ptr newDirectory =
      make_shared<inode>(file_type::DIRECTORY_TYPE);
   dirents.emplace(dirname, newDirectory);
   return newDirectory;
}

inode_ptr directory::mkfile(const string& filename) {
   DEBUGF('i', filename);
   inode_ptr newFilename =
      make_shared<inode>(file_type::PLAIN_TYPE);
   dirents.emplace(filename, newFilename);
   return newFilename;
}

// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $
//-- Constantine Kolokousis (kkolokou@ucsc.edu)
//-- Moises Perez(mperez86@ucsc.edu)
#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   while (begin() != end()) { erase(begin()); }
   anchor_.next = anchor_.prev = NULL;
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   auto i = begin();
   //Find spot
   for (; i != end(); ++i) {
      if (less(pair.first, i->first)) {
         break;
      }
   }
   node* n = new node(i.where, i.where->prev, pair);
   n->next->prev = n;
   n->prev->next = n;
   return i;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   auto i = begin();
   for (; i != end(); ++i) {
      if (!less(that, i->first))
         if (!less(i->first, that))
            break;
   }
   return i;
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);

   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;

   iterator temp = iterator(position.where->next);
   delete(position.where);
   position.where = NULL;

   return temp;
}

template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, less_t>::printLL() {
   for (auto i = this->begin(); i != this->end(); ++i) {
      cout << i->first << " = " << i->second << endl;
   }
}

template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, less_t>::printPair
(const mapped_type& pair) {
   for (auto i = this->begin(); i != this->end(); ++i) {
      if (i->second != pair) continue;
      cout << i->first << " = " << i->second << endl;
   }
}

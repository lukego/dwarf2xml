/* 
 * Copyright (C) 2008-2009 by Emmanuel Azencot under the GNU LGPL
 * license version 2.0 or 2.1.  You should have received a copy of the
 * LGPL license along with this library if you did not you can find it
 * at http://www.gnu.org/.
 */
/*
 * Azencot : Wed Nov 12 21:45:57 CET 2008
 *  Creation
 * Azencot : Tue Nov 18 21:31:30 CET 2008
 *  Add doc++ comments
 * Azencot : Sat Dec 13 00:20:02 CET 2008
 *  f_ring_off_xxx -> f_ring_xxx
 */
#define _GNU_SOURCE
#include <errno.h>
#include <string.h>

#include "config.h"
#include "exception.h"
#include "ring.h"

/** @memo Hold check option. */
static int check_opt;
/** @memo Set or get check option.
 * @doc
 *  The check option provides integrity check on the ring on each call to
 * API funtions.
 * @param level (IN) 0 set to disable, 1 enable, -1 read curent value.
 * @return curent value.
 */
int f_ring_check_opt(int level) { if ( level != -1 ) check_opt = level; return check_opt; }

/** @name Ring
 */
//@{
/** @memo Ring self integrity test
 * @doc
 *  Walk around the ring to check link ptrs.
 *  In most of the errors situation the functions will not give or even
 * cleanly return.
 * @param ring (IN) Ring to be checked
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_ring_selftest(struct s_ring *ring, size_t offset) {
   typeof(ring) r;
   if ( ring == (typeof(ring))offset ) return 1;
   for ( r = ring; r; r = r->next ) {
      if ( r == ring ) return 1;
   }
   raise (return 0, EMLINK, "nul node found in ring");
}
/** @memo Test if an element is in a ring
 * @doc
 *  Return true if element is in.
 * @param is (IN) Element to search
 * @param in (IN) Ring
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EFAULT is is nil.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_ring_is_in(struct s_ring *is, struct s_ring *in, size_t offset) {
   typeof(in) r;
   if ( is == (typeof(in))offset ) raise(return 0, EFAULT, "is, is nil, indeed");
   if ( in == (typeof(in))offset ) return 0;
   if ( check_opt > 0 && !f_ring_selftest(in, offset) ) relay (return 0, " ") ;
   r = in;
   do {
      if ( r == is ) return 1;
   } while ( r = r->next, r != in );
   return 0;
}
/** @memo Add a new element in the ring.
 * @doc
 *  Insert a new pearl in a ring. 
 *  Return is always valid, even if ring is nil.
 * Be carefull if new can be nil, as you can lose the ring handle.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param new (IN) a pointer to the ring part of the node
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EEXIST, new is already in ring.
 * @exception EFAULT new is nil.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_ring_link(struct s_ring *ring, struct s_ring *new, size_t offset) {
   if ( check_opt > 0 && !f_ring_selftest(ring, offset) == 0 ) relay (return 0, "Memory jam");

   if ( new == (typeof(ring))offset ) raise (return 0, EFAULT, "new node is nil");
   if ( check_opt > 0 && f_ring_is_in(new, ring, offset) ) 
      raise (return 0, EEXIST, "node is already in ring");

   if ( ring == (typeof(ring))offset ) ring = new;

   new->next = ring->next;
   ring->next = new;

   return new?((char *)new - offset):0;
}
/** @memo Remove an element from a ring
 * @doc
 *   The ring is relinked to exclude the element.
 * Return ptr is 0 if ring becomes empty.
 * @param node (IN) a pointer to ring part of the node 
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_ring_unlink(struct s_ring *node, size_t offset) {
   typeof(node) r;

   if ( check_opt > 0 && !f_ring_selftest(node, offset) == 0) relay (return 0, "Memory jam");
   if ( node == (typeof(node))offset ) return 0;
   if ( node == node->next ) return 0;

   for ( r = node; r->next != node; r = r->next );
   r->next = node->next;
   node->next = 0;

   return r?((char *)r - offset):0;
}
//@}
/** @name Named ring
 */
//@{
/** @memo Name ring integrity test
 * @doc
 *  Walk around the ring to check link ptrs.
 *  In most of the errors situation the functions will not give or even
 * cleanly return.
 * @param ring (IN) Ring to be checked
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_nring_selftest(struct s_nring *ring, size_t offset) {
   typeof(ring) r;
   if ( ring == (typeof(ring))offset ) return 1;
   for ( r = ring; r; r = r->next ) {
      if ( r == ring ) return 1;
   }
   raise (return 0, EMLINK, "nul node found in ring");
}
/** @memo Test if an element is in a ring
 * @doc
 *  Return true if element is in.
 * @param is (IN) Element to search
 * @param in (IN) Ring
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EFAULT is is nil.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_nring_is_in(struct s_nring *is, struct s_nring *in, size_t offset) {
   typeof(in) r;
   if ( is == (typeof(in))offset ) raise(return 0, EFAULT, "is, is nil, indeed");
   if ( in == (typeof(in))offset ) return 0;
   if ( check_opt > 0 && !f_nring_selftest(in, offset) ) relay (return 0, " ") ;
   r = in;
   do {
      if ( r == is ) return 1;
   } while ( r = r->next, r != in );
   return 0;
}
/** @memo Find a node in a named ring by its name.
 * @doc
 *   Names are limited to 1000 bytes.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param name (IN) name to look for
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is not found, or a node.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_nring_find(struct s_nring *ring, const char *name, size_t offset) {
   typeof(ring) r;
   if ( check_opt > 0 && !f_nring_selftest(ring, offset) == 0) relay (return 0, " ");
   if ( ring == (typeof(ring))offset ) return 0;

   for ( r = ring->next; r && strncmp(r->name, name, 1000); r = r->next ) {
      if ( r == ring ) return 0;
   }
   return (r)?((char *)r - offset):0;
}
/** @memo Add a new element in the ring.
 * @doc
 *  Insert a new pearl in a ring. 
 *  Return is always valid, even if ring is nil.
 * Be carefull if new can be nil, as you can lose the ring handle.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param new (IN) a pointer to the ring part of the node
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception ENOTNAM, Name is nil.
 * @exception ENAMETOOLONG, Name is too long.
 * @exception EEXIST, new is already in ring.
 * @exception EFAULT new is nil.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_nring_link(struct s_nring *ring, struct s_nring *new, size_t offset) {
   if ( check_opt > 0 && !f_nring_selftest(ring, offset) == 0 ) relay (return 0, "Memory jam");

   if ( new == (typeof(ring))offset ) raise (return 0, EFAULT, "new node is nil");
   if ( !new->name ) raise (return 0, ENOTNAM, "new node name is nil");
   if ( strnlen(new->name, 1000) == 1000 ) raise (return 0, ENAMETOOLONG, "new node name is too long");
   if ( check_opt > 0 && f_nring_is_in(new, ring, offset) ) 
      raise (return 0, EEXIST, "node is already in ring");
   if ( f_nring_find(ring, new->name, offset) ) 
      raise(return 0, ENOTUNIQ, "A node named \"%s\" already exists",new->name);

   if ( ring == (typeof(ring))offset ) ring = new;

   new->next = ring->next;
   ring->next = new;

   return new?((char *)new - offset):0;
}
/** @memo Remove an element from a ring
 * @doc
 *   The ring is relinked to exclude the element.
 * Return ptr is 0 if ring becomes empty.
 * @param node (IN) a pointer to ring part of the node 
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_nring_unlink(struct s_nring *node, size_t offset) {
   typeof(node) r;

   if ( check_opt > 0 && !f_nring_selftest(node, offset) == 0) relay (return 0, "Memory jam");
   if ( node == (typeof(node))offset ) return 0;
   if ( node == node->next ) return 0;

   for ( r = node; r->next != node; r = r->next );
   r->next = node->next;

   return r?((char *)r - offset):0;
}
//@}

/** @name Double linked ring
 */
//@{
/** @memo Ring integrity test
 * @doc
 *  Walk around the ring to check link ptrs.
 *  In most of the errors situation the functions will not give or even
 * cleanly return.
 * @param ring (IN) Ring to be checked
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_dring_selftest(struct s_dring *ring, size_t offset) {
   typeof(ring) r;

   if ( ring == (typeof(ring))offset ) return 1;
   
   for ( r = ring; r; r = r->next ) {
      if ( !r->next ) break;
      if ( !r->prev ) break;
      if ( r->next->prev != r || r->prev->next != r)
         raise (return 0, EMLINK, "Broken link for node %p", r);
      if ( r == ring ) return 1;
   }
   raise (return 0, EMLINK, "Nul link found in node %p", r);
}
/** @memo Test if an element is in a ring
 * @doc
 *  Return true if element is in.
 * @param is (IN) Element to search
 * @param in (IN) Ring
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EFAULT is is nil.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_dring_is_in(struct s_dring *is, struct s_dring *in, size_t offset) {
   typeof(in) r;
   if ( is == (typeof(in))offset ) raise(return 0, EFAULT, "is, is nil, indeed");
   if ( in == (typeof(in))offset ) return 0;
   if ( check_opt > 0 && !f_dring_selftest(in, offset) ) relay (return 0, " ") ;
   r = in;
   do {
      if ( r == is ) return 1;
   } while ( r = r->next, r != in );
   return 0;
}
/** @memo Return +/- nth node frome curent
 * @doc
 *  Provide a abitrary position displacement in a ring. It is probably
 * usefull only for small values of hop counts (1 or -1), as ring order
 * does not matter.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param hops (IN) number of hops to move
 * @param check (IN) Check circular overshoot
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty or check error, node at pos + hops in the ring.
 * @exception ELOOP Move made a complete loop
 * @exception EMLINK null pointer found in ring link.
 */
void *f_dring_move(struct s_dring *ring, int hops, bool check, size_t offset) {
   typeof(ring) r = ring;

   if ( ring == (typeof(ring))offset || !hops ) return (char *)ring -offset;
   if ( check_opt > 0 && !f_dring_selftest(ring, offset) ) relay (return 0, " ") ;
   if ( hops > 0 ) {
      for ( r = ring->next, --hops; hops; --hops, r = r->next ) {
         if ( check && r->next == ring ) raise( return (void *) -offset, ELOOP, "Move made a complete loop"); 
      }
      return (char *)r - offset;
   }
   for ( r = ring->prev, ++hops; hops; ++hops, r = r->prev ) {
      if ( check && r->prev == ring ) raise( return (void *) -offset, ELOOP, "Move made a complete loop"); 
   }
   return (char *)r - offset;
}
/** @memo Add a new element in the ring.
 * @doc
 *  Insert a new pearl in a ring. 
 *  Return is always valid, even if ring is nil.
 * Be carefull if new can be nil, as you can lose the ring handle.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param new (IN) a pointer to the ring part of the node
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EEXIST, new is already in ring.
 * @exception EFAULT new is nil.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_dring_link(struct s_dring *ring, struct s_dring *new, size_t offset) {
   if ( check_opt > 0 && !f_dring_selftest(ring, offset) == 0 ) relay (return 0, " ");
   if ( new == (typeof(ring))offset ) raise (return 0, EFAULT, "new node is nil");
   if ( check_opt > 0 && f_dring_is_in(new, ring, offset) ) 
      raise (return 0, EEXIST, "node is already in ring");
   if ( ring == (typeof(ring))offset ) ring = new; 

   new->next = ring->next;
   new->prev = ring;
   ring->next = new;
   new->next->prev = new;

   return new?((char *)new - offset):0;
}
/** @memo Remove an element from a ring
 * @doc
 *   The ring is relinked to exclude the element.
 * Return ptr is 0 if ring becomes empty.
 * @param node (IN) a pointer to ring part of the node 
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_dring_unlink(struct s_dring *node, size_t offset) {
   if ( node == (typeof(node))offset ) return 0; /* node = 0 */

   /* AZT sam, 09 mai 2009 13:00:46 +0200 
   if ( node == (typeof(node))offset ) return 0; */
   if ( check_opt > 0 && !f_dring_selftest(node, offset) == 0) relay (return 0, " ");
   if ( node == node->next ) return 0;

   node->prev->next = node->next;
   node->next->prev = node->prev;

   return node?((char *)node->prev - offset):0;
}
//@}

/** @name Double linked named ring
 */
//@{
/** @memo Ring integrity test
 * @doc
 *  Walk around the ring to check link ptrs.
 *  In most of the errors situation the functions will not give or even
 * cleanly return.
 * @param ring (IN) Ring to be checked
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_ndring_selftest(struct s_ndring *ring, size_t offset) {
   typeof(ring) r;

   if ( ring == (typeof(ring))offset ) return 1;
   
   for ( r = ring; r; r = r->next ) {
      if ( !r->next ) break;
      if ( !r->prev ) break;
      if ( r->next->prev != r || r->prev->next != r)
         raise (return 0, EMLINK, "Broken link for node %p", r);
      if ( r == ring ) return 1;
   }
   raise (return 0, EMLINK, "Nul link found in node %p", r);
}
/** @memo Test if an element is in a ring
 * @doc
 *  Return true if element is in.
 * @param is (IN) Element to search
 * @param in (IN) Ring
 * @param offset (IN) offset of structure s_ring
 * @return 1 if OK, 0 and errno if faulty.
 * @exception EFAULT is is nil.
 * @exception EMLINK null pointer found in ring link.
 */ 
bool f_ndring_is_in(struct s_ndring *is, struct s_ndring *in, size_t offset) {
   typeof(in) r;
   if ( is == (typeof(in))offset ) raise(return 0, EFAULT, "is, is nil, indeed");
   if ( in == (typeof(in))offset ) return 0;
   if ( check_opt > 0 && !f_ndring_selftest(in, offset) ) relay (return 0, " ") ;
   r = in;
   do {
      if ( r == is ) return 1;
   } while ( r = r->next, r != in );
   return 0;
}

/** @memo Return +/- nth node frome curent
 * @doc
 *  Provide a abritrary position displacement in a ring. It is probably
 * usefull only for small values of hop counsts (1 or -1), as ring order
 * does not matter.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param hops (IN) number of hops to move
 * @param check (IN) Check circular overshoot
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty or check error, node at pos + hops in the ring.
 * @exception ELOOP Move made a complete loop
 * @exception EMLINK null pointer found in ring link.
 */
void *f_ndring_move(struct s_ndring *ring, int hops, bool check, size_t offset) {
   typeof(ring) r = ring;

   if ( ring == (typeof(ring))offset || !hops ) return (char *)ring -offset;
   if ( check_opt > 0 && !f_ndring_selftest(ring, offset) ) relay (return 0, " ") ;
   if ( hops > 0 ) {
      for ( r = ring->next, --hops; hops; --hops, r = r->next ) {
         if ( check && r->next == ring ) raise( return (void *) -offset, ELOOP, "Move made a complete loop"); 
      }
      return (char *)r - offset;
   }
   for ( r = ring->prev, ++hops; hops; ++hops, r = r->prev ) {
      if ( check && r->prev == ring ) raise( return (void *) -offset, ELOOP, "Move made a complete loop"); 
   }
   return (char *)r - offset;
}
/** @memo Find a node in a named ring by its name.
 * @doc
 *   Names are limited to 1000 bytes.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param name (IN) name to look for
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is not found, or a node.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_ndring_find(struct s_ndring *ring, const char *name, size_t offset) {
   typeof(ring) r;
   if ( check_opt > 0 && !f_ndring_selftest(ring, offset) == 0) relay (return 0, " ");
   if ( ring == (typeof(ring))offset ) return 0;

   for ( r = ring->next; r && strncmp(r->name, name, 1000); r = r->next ) {
      if ( r == ring ) return 0;
   }
   return (r)?((char *)r - offset):0;
}
/** @memo Add a new element in the ring.
 * @doc
 *  Insert a new pearl in a ring. 
 *  Return is always valid, even if ring is nil.
 * Be carefull if new can be nil, as you can lose the ring handle.
 * @param ring (IN) a pointer to ring part of a ring's node 
 * @param new (IN) a pointer to the ring part of the node
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception ENOTNAM, Name is nil.
 * @exception ENAMETOOLONG, Name is too long.
 * @exception EEXIST, new is already in ring.
 * @exception EFAULT new is nil.
 * @exception EMLINK null pointer found in ring link.
 */ 
void *f_ndring_link(struct s_ndring *ring, struct s_ndring *new, size_t offset) {
   if ( check_opt > 0 && !f_ndring_selftest(ring, offset) == 0 ) relay (return 0, " ");
   if ( new == (typeof(ring))offset ) raise (return 0, EFAULT, "new node is nil");
   if ( !new->name ) raise (return 0, ENOTNAM, "new node name is nil");
   if ( strnlen(new->name, 1000) == 1000 ) raise (return 0, ENAMETOOLONG, "new node name is too long");
   if ( check_opt > 0 && f_ndring_is_in(new, ring, offset) ) 
      raise (return 0, EEXIST, "node is already in ring");
   if ( f_ndring_find(ring, new->name, offset) ) 
      raise(return 0, ENOTUNIQ, "A node named \"%s\" already exists",new->name);

   if ( ring == (typeof(ring))offset ) ring = new; 

   new->next = ring->next;
   new->prev = ring;
   ring->next = new;
   new->next->prev = new;

   return new?((char *)new - offset):0;
}
/** @memo Remove an element from a ring
 * @doc
 *   The ring is relinked to exclude the element.
 * Return ptr is 0 if ring becomes empty.
 * @param node (IN) a pointer to ring part of the node 
 * @param offset (IN) the offset of the ring part in the node.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EMLINK null pointer found in ring link.
 */
void *f_ndring_unlink(struct s_ndring *node, size_t offset) {
   if ( node == (typeof(node))offset ) node = 0;

   if ( node == (typeof(node))offset ) return 0;
   if ( check_opt > 0 && !f_ndring_selftest(node, offset) == 0) relay (return 0, " ");
   if ( node == node->next ) return 0;

   node->prev->next = node->next;
   node->next->prev = node->prev;

   return node?((char *)node->prev - offset):0;
}

//@}


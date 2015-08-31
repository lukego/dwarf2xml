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
#ifndef _RING_H_ /* [ */
#define _RING_H_

#include <stddef.h> /* offsetof */
#include <stdbool.h>

/**@name Structures
 * @doc
 *  The easiest way to use rings is to include a ring structure in your C object 
 * structure :
 * <pre>
     struct s_your_object {
        type member;
        ...
        struct s_ring sibling;
        type member;
        ....
     };</pre>
 *  The set of macros use the ring structure member name to find the offset of
 * the ring structure in yours.
 *  These macros try to keep type informations as far as possible for the compiler
 * to be able to check them.
 */
//@{
/** @memo The ring structure
 * @doc
 *  You must include in your own structure at the same offset, for all nodes
 * of the same ring.
 * It is recommended to include the ring structure directly in the node one.
 * The macros need a member reference to find the ring part.
 */
struct s_ring { struct s_ring *next; };
/** @memo double linked object
 * @doc
 *  These kind of rings provides 2 extar methodes : previous and move..
 */
struct s_dring { struct s_dring *next; struct s_dring *prev; };
/** @memo Include this structure for named object
 * @doc
 *  The node name is a null terminated string pointer. Caller is responsible 
 * of memory management for the name.
 *  Name must be less than 1000 characters.
 */
struct s_nring { struct s_nring *next; const char *name; };
/** @memo Double linked named object
 * @doc
 *  The node name is a null terminated string pointer. Caller is responsible 
 * of memory management for the name.
 *  Name must be less than 1000 characters.
 */
struct s_ndring { struct s_ndring *next; struct s_ndring *prev; char *name; };
//@}
/**@name Selftest (macro)
 * @memo The ring integrity routine.
 * @doc
 *  These macros call the same routine that is it used when check_opt is set.
 * Feel free to call it in case you have disabled this check.
 * @param p_ring (IN) A holding pointer to a node's ring
 * @param field (IN) Field member name of ring structure
 * @return 0 is failled
 * @exception EMLINK null pointer found in ring link.
 */
//@{
/// ring
#define m_ring_selftest(p_ring, field) ( \
   (typeof(p_ring))f_ring_selftest(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
/// nring
#define m_nring_selftest(p_ring, field) ( \
   (typeof(p_ring))f_nring_selftest(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
/// dring
#define m_dring_selftest(p_ring, field) ( \
   (typeof(p_ring))f_dring_selftest(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
/// ndring
#define m_ndring_selftest(p_ring, field) ( \
   (typeof(p_ring))f_ndring_selftest(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
//@}
/**@name Test node is in a ring (macro)
 * @memo Is this node in the this ring ?.
 * @doc
 *   Lost your ring ? Just try all you have with this method.
 * + missing macro.
 * @param is (IN) this node is 
 * @param in (IN) this ring
 * @return 0 if not
 * @exception EFAULT is is nil.
 * @exception EMLINK null pointer found in ring link.
 */
//@{
/// ring
#define m_ring_is_in(is, in, field)  (f_ring_is_in(&((is)->field), &((in)->field), offsetof(typeof(*(in)), field) ) )
/// nring
#define m_nring_is_in(is, in, field) (f_nring_is_in(&((is)->field), &((in)->field), offsetof(typeof(*(in)), field) ) )
/// dring
#define m_dring_is_in(is, in, field) (f_dring_is_in(&((is)->field), &((in)->field), offsetof(typeof(*(in)), field) ) )
/// ndring
#define m_ndring_is_in(is, in, field) (f_ndring_is_in(&((is)->field), &((in)->field), offsetof(typeof(*(in)), field) ) )
//@}
/**@name Link (macro)
 * @memo Hold a new node in the ring.
 * @doc
 *   if ring reference is nul, the operation succed. To avoid side effect on empty rings,
 * always set the ring holding pointer with the result of the call :
 * <pre>ring_ptr = m_ring_link(ring_ptr, field, node);</pre>
 * @param p_ring (IN) a pointer to your structure node 
 * @param field (IN) the ring structure field name in the node structure.
 * @param node (IN) the node
 * @return node in the now in ring.
 * @exception ENOTNAM, Name is nil (nring, ndring).
 * @exception ENAMETOOLONG, Name is too long (nring, ndring).
 * @exception EEXIST, node is already in ring.
 * @exception EFAULT node is nil.
 * @exception EMLINK null pointer found in ring link.
 */
//@{
/// ring
#define m_ring_link(p_ring, field, node) ( (typeof(p_ring))f_ring_link(&((p_ring)->field), &((node)->field), offsetof(typeof(*(p_ring)), field) ) )
/// nring
#define m_nring_link(p_ring, field, node) ( (typeof(p_ring))f_nring_link(&((p_ring)->field), &((node)->field), offsetof(typeof(*(p_ring)), field) ) )
/// dring
#define m_dring_link(p_ring, field, node) ( (typeof(p_ring))f_dring_link(&((p_ring)->field), &((node)->field), offsetof(typeof(*(p_ring)), field) ) )
/// ndring
#define m_ndring_link(p_ring, field, node) ( (typeof(p_ring))f_ndring_link(&((p_ring)->field), &((node)->field), offsetof(typeof(*(p_ring)), field) ) )
//@}
/**@name Unlink (macro)
 * @memo Remove reference from this node.
 * @doc
 *  Because the holding pointer may reference the unlinked node, always set the ring
 * holding pointer with the result of the call :
 *  <pre>ring_ptr = m_ring_unlink(node, field);</pre>
 *  It is highly recomanded to ensure that the node belongs to ring_ptr before calling
 * the macro.
 * @param p_ring (IN) The node you want to unlink.
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the next node in the ring.
 * @exception EMLINK null pointer found in ring link.
 */
//@{
/// ring
#define m_ring_unlink(p_ring, field) ( (typeof(p_ring))f_ring_unlink(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
/// nring
#define m_nring_unlink(p_ring, field) ( (typeof(p_ring))f_nring_unlink(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
/// dring
#define m_dring_unlink(p_ring, field) ( (typeof(p_ring))f_dring_unlink(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
/// ndring
#define m_ndring_unlink(p_ring, field) ( (typeof(p_ring))f_ndring_unlink(&((p_ring)->field), offsetof(typeof(*(p_ring)), field) ) )
//@}

/**@name Next (macro)
 * @memo the next node.
 * @doc
 *   The macro expand as the next node of a node, allowing you to jump each node.
 * Note that you have to provide a stop condition yourself when you loop around the ring.
 * @param p_ring (IN) a pointer to structure node 
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the next node in the ring.
 */
//@{
/// ring
#define m_ring_next(p_ring, field)   ((typeof(p_ring))((p_ring)?((char *)((p_ring)->field.next) -  offsetof(typeof(*(p_ring)), field) ) :0))
/// nring
#define m_nring_next(p_ring, field)  ((typeof(p_ring))((p_ring)?((char *)((p_ring)->field.next) -  offsetof(typeof(*(p_ring)), field) ) :0))
/// dring
#define m_dring_next(p_ring, field)  ((typeof(p_ring))((p_ring)?((char *)((p_ring)->field.next) -  offsetof(typeof(*(p_ring)), field) ) :0))
/// ndring
#define m_ndring_next(p_ring, field) ((typeof(p_ring))((p_ring)?((char *)((p_ring)->field.next) -  offsetof(typeof(*(p_ring)), field) ) :0))
//@}

/**@name List (macro)
 * @memo the next node, with loop detection.
 * @doc
 *   The macro expand as the next node of a node, allowing you to jump each node.
 *   If next node is ring parameter, it returns 0. This macro is switable for loops :
 *
 *     for (node = ring; !node; node = m_ring_list(list, node, brother) ) { ... }
 *
 * @param p_ring (IN) a pointer to structure node 
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the next node in the ring.
 */
//@{
/// ring
#define m_ring_list(ring, node, field) ( (((node) = m_ring_next(node, field)) == (ring))?0:(node) )
/// nring
#define m_nring_list(ring, node, field) ( (((node) = m_nring_next(node, field)) == (ring))?0:(node) )
/// dring
#define m_dring_list(ring, node, field) ( (((node) = m_dring_next(node, field)) == (ring))?0:(node) )
/// ndring
#define m_ndring_list(ring, node, field) ( (((node) = m_ndring_next(node, field)) == (ring))?0:(node) )
//@}
/**@name Do, done (macro)
 * @memo A loop macro construction helper.
 * @doc
 *   These to macros construct begin and end of loops to evry node in a ring :
 *  <pre>m_ring_do(ring, node) ++i; m_ring_done(ring, node, field);</pre>
 * count how many node there is in the ring.
 * @param ring (IN) a pointer the ring ring 
 * @param var (IN) a variable pointer node beeing listed.
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the next node in the ring.
 */
//@{
/// ring, start loop
#define m_ring_do(ring, var) if ( ((var) = (ring)) ) do
/// ring, end loop
#define m_ring_done(ring, var, field) while ( (var) = m_ring_next((var), field), (var) != (ring) )
/// nring, start loop
#define m_nring_do(ring, var) if ( ((var) = (ring)) ) do
/// nring, end loop
#define m_nring_done(ring, var, field) while ( (var) = m_nring_next((var), field), (var) != (ring) )
/// dring, start loop
#define m_dring_do(ring, var) if ( ((var) = (ring)) ) do
/// dring, end loop
#define m_dring_done(ring, var, field) while ( (var) = m_dring_next((var), field), (var) != (ring) )
/// ndring, start loop
#define m_ndring_do(ring, var) if ( ((var) = (ring)) ) do
/// ndring, end loop
#define m_ndring_done(ring, var, field) while ( (var) = m_ndring_next((var), field), (var) != (ring) )
//@}
/**@name Find by name (macro)
 * @memo For named ring, find a node with his name.
 * @doc
 * @param p_ring (IN) a pointer to your structure node 
 * @param field (IN) the ring structure field name in the node structure.
 * @param name (IN) the node name.
 * @return 0 the name is not found, the node if it is.
 * @exception EMLINK null pointer found in ring link.
 */
//@{
/// nring
#define m_nring_find(p_ring, field, name)  ( \
   (typeof(p_ring))f_nring_find(&((p_ring)->field), (name), offsetof(typeof(*(p_ring)), field) ) )
/// ndring
#define m_ndring_find(p_ring, field, name)  ( \
   (typeof(p_ring))f_ndring_find(&((p_ring)->field), (name), offsetof(typeof(*(p_ring)), field) ) )
//@}
/**@name Previous (macro)
 * @memo For double ring, expand to previous node.
 * @doc
 * @param p_ring (IN) a pointer to your structure node 
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the previous node in the ring.
 */
//@{
/// dring
#define m_dring_prev(p_ring, field)  ((typeof(p_ring))((p_ring)?((char *)((p_ring)->field.prev) -  offsetof(typeof(*(p_ring)), field) ) :0))
/// ndring
#define m_ndring_prev(p_ring, field) ((typeof(p_ring))((p_ring)?((char *)((p_ring)->field.prev) -  offsetof(typeof(*(p_ring)), field) ) :0))
//@}
/**@name Move (macro)
 * @memo For double ring,jump n nodes back or force.
 * @doc
 * @param p_ring (IN) a pointer to your structure node 
 * @param hops (IN) Number of jumps.
 * @param check (IN) if true, a loop detection.
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the previous node in the ring.
 * @exception ELOOP Move made a complete loop
 * @exception EMLINK null pointer found in ring link.
 */
//@{
/// nring
#define m_dring_move(p_ring, hops, check, field) ( \
   (typeof(p_ring))f_dring_move(&((p_ring)->field), hops, check, offsetof(typeof(*(p_ring)), field) ) )
/// ndring
#define m_ndring_move(p_ring, hops, check, field) ( \
   (typeof(p_ring))f_ndring_move(&((p_ring)->field), hops, check, offsetof(typeof(*(p_ring)), field) ) )
//@}
/**@name Errors
 * @doc
 *   When an error is detected, errno is used to give a minimal feed back. Errno values
 * are given in each macro and function in there respectives manual entries at
 * paragraphs "throws".
 *  It is possible to customise error signaling by rewriting raise and relay macros
 *  in "config.h" file. This way, you could get textual reason. 
 *  See directory \URL{../excp} for more information.
 * @exception ELOOP Move made a complete loop (dring, ndring)
 * @exception ENOTNAM Name is nil (nring, ndring).
 * @exception ENAMETOOLONG Name is too long (nring, ndring).
 * @exception EEXIST node is already in ring.
 * @exception EMLINK null pointer found in ring link.
 * @exception EFAULT node is nil.
 */
//@{
//@}
/**@name Maintenance
 */
//@{
/** @memo Set or get the check bit control on
 * @doc
 *   When check is on, the linkage integrity is checked each time a member
 * function is called.
 *   To get the curent check level, call with level at -1.
 * @param level (IN) Only two levels : 0 or 1. -1 for read.
 * @return curent check level.
 */
int f_ring_check_opt(int level);
//@}
/**@name C API 
 * @memo The C fonctions set that implement rings
 * @doc
 *  The C API is less "friend user" than macros because more code have to be writen
 * to call them properly.
 *  In some situations where no member name can be provided to the macros for the ring
 * datas they may be usefull.
 * See \Ref{Implementation}
 */
//@{
bool f_ring_selftest(struct s_ring *ring, size_t offset);
bool f_dring_selftest(struct s_dring *ring, size_t offset);
bool f_nring_selftest(struct s_nring *ring, size_t offset);
bool f_ndring_selftest(struct s_ndring *ring, size_t offset);

bool f_ring_is_in(struct s_ring *is, struct s_ring *in, size_t offset);
bool f_nring_is_in(struct s_nring *is, struct s_nring *in, size_t offset);
bool f_dring_is_in(struct s_dring *is, struct s_dring *in, size_t offset);
bool f_ndring_is_in(struct s_ndring *is, struct s_ndring *in, size_t offset);

void *f_ring_unlink(struct s_ring *node, size_t offset);
void *f_nring_unlink(struct s_nring *node, size_t offset);
void *f_dring_unlink(struct s_dring *node, size_t offset);
void *f_ndring_unlink(struct s_ndring *node, size_t offset);

void *f_ring_link(struct s_ring *ring, struct s_ring *new, size_t offset);
void *f_nring_link(struct s_nring *ring, struct s_nring *new, size_t offset);
void *f_dring_link(struct s_dring *ring, struct s_dring *new, size_t offset);
void *f_ndring_link(struct s_ndring *ring, struct s_ndring *new, size_t offset);

void *f_nring_find(struct s_nring *ring, const char *name, size_t offset);
void *f_ndring_find(struct s_ndring *ring, const char *name, size_t offset);

void *f_dring_move(struct s_dring *ring, int hops, bool check, size_t offset);
void *f_ndring_move(struct s_ndring *ring, int hops, bool check, size_t offset);
//@}

/*
struct s_ring *f_ring_next(struct s_ring *node);
struct s_ring *f_ring_link(struct s_ring *ring, struct s_ring *node);
struct s_ring *f_ring_unlink(struct s_ring *node);

struct s_nring *f_nring_next(struct s_nring *ring);
struct s_nring *f_nring_link(struct s_nring *ring, struct s_nring *new);
struct s_nring *f_nring_unlink(struct s_nring *node);
struct s_nring *f_nring_find(struct s_nring *ring, const char *name);

struct s_dring *f_dring_next(struct s_dring *ring);
struct s_dring *f_dring_prev(struct s_dring *ring);
struct s_dring *f_dring_move(struct s_dring *ring, int hops, bool check);

struct s_dring *f_dring_link(struct s_dring *ring, struct s_dring *new);
struct s_dring *f_dring_unlink(struct s_dring *node);
** @memo Internal, expand to the ring structure adresse of a node.
 * @doc
 *   The macro expand as the next node of a node, allowing you to jump each node.
 * Note that you have to provide a stop condition yourself when you loop around the ring.
 * @param p_ring (IN) a pointer to structure node 
 * @param field (IN) the ring structure field name in the node structure.
 * @return 0 the ring is empty, or the next node in the ring.
 *
#define m_ring_to_container(p_ring, field) ((typeof(p_ring))((p_ring)?((char *)(p_ring) - offsetof(typeof(*(p_ring)), field) ) :0))


*/

#endif /* ] _RING_H_ */

/*
 *  $Id: s_gqegdp.c,v 1.1 1997-03-05 19:12:57 haley Exp $
 */
/*
 *  Inquire list of available generalized drawing primitives  
 */

#include <ncarg/gks.h>

void ginq_list_avail_gdps
#ifdef NeedFuncProto
(
    Gint      ws_type,             /* workstation identifier     */
    Gint      num_elems_appl_list, /* length of application list */
    Gint      start_pos,           /* starting position          */
    Gint      *err_ind,            /* OUT error indicator        */
    Gint_list *gdp,                /* OUT list of GDPs           */
    Gint      *length_list         /* OUT length of list in GKS  */
)
#else
(ws_type,num_elems_appl_list,start_pos,err_ind,gdp,length_list)
    Gint      ws_type;
    Gint      num_elems_appl_list;
    Gint      start_pos;
    Gint      *err_ind;
    Gint_list *gdp;
    Gint      *length_list;
#endif
{
/*  Note:  This routine does not do anything at this point because
 *         the NCARG GKS package does not use generalized drawing
 *         primitives.  If this changes in the future, then this
 *         routine will be modified accordingly.
 */
}

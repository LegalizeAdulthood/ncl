/*
 *      $Id: MapV41DataHandler.c,v 1.2 1998-05-25 18:52:07 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Thu Apr 23 12:01:04 MDT 1998
 *
 *	Description:	
 */

#include <ncarg/hlu/MapV41DataHandlerP.h>

static NhlErrorTypes MapV41DHClassPartInit(
#if	NhlNeedProto
	NhlClass	lc
#endif
);


static NhlErrorTypes MapV41DHInitialize(
#if	NhlNeedProto
        NhlClass	class,
        NhlLayer	req,
        NhlLayer	new,
        _NhlArgList	args,
        int             num_args
#endif
);

static NhlErrorTypes  MapV41DHSetValues(
#if	NhlNeedProto
        NhlLayer	old,
        NhlLayer	reference,
        NhlLayer	new,
        _NhlArgList	args,
        int             num_args
#endif
);

static NhlErrorTypes    MapV41DHGetValues(
#if	NhlNeedProto
	NhlLayer        l,
	_NhlArgList     args,
	int             num_args
#endif
);

static NhlErrorTypes    MapV41DHDestroy(
#if	NhlNeedProto
	NhlLayer        l
#endif
);

static NhlErrorTypes MapV41DHUpdateDrawList(
#if	NhlNeedProto
	NhlLayer		instance,
        NhlBoolean  		init,
        NhlMapPlotLayer 	newmp,
        NhlMapPlotLayer 	oldmp,
        _NhlArgList		args,
        int             	num_args
#endif
);

static NhlErrorTypes MapV41DHDrawMapList(
#if	NhlNeedProto
	NhlLayer		instance,
        NhlMapPlotLayer 	mp,
        mpDrawOp		draw_op,
	NhlBoolean		init_draw
#endif
);


static void   load_hlumap_routines(
#if	NhlNeedProto
	NhlBoolean	flag
#endif
);

void   (_NHLCALLF(hlumapeod,HLUMAPEOD))(
#if	NhlNeedProto
	int *nout,
	int *nseg,
	int *idls,
	int *idrs,
	int *npts,
	float *pnts
#endif
);

NhlMapV41DataHandlerClassRec NhlmapV41DataHandlerClassRec = {
	{
/* class_name 		*/      "mapV41DataHandlerClass",
/* nrm_class 		*/      NrmNULLQUARK,
/* layer_size 		*/      sizeof(NhlMapV41DataHandlerLayerRec),
/* class_inited 	*/	False,
/* superclass		*/      (NhlClass)&NhlmapDataHandlerClassRec,
/* cvt_table		*/	NULL,

/* layer_resources 	*/   	NULL,
/* num_resources 	*/     	0,
/* all_resources 	*/	NULL,
/* callbacks		*/	NULL,
/* num_callbacks	*/	0,
/* class_callbacks	*/	NULL,
/* num_class_callbacks	*/	0,

/* class_part_initialize */     MapV41DHClassPartInit,
/* class_initialize 	*/  	NULL,
/* layer_initialize 	*/  	MapV41DHInitialize,
/* layer_set_values 	*/  	MapV41DHSetValues,
/* layer_set_values_hook */  	NULL,
/* layer_get_values 	*/  	MapV41DHGetValues,
/* layer_reparent 	*/  	NULL,
/* layer_destroy 	*/    	MapV41DHDestroy,
	},
	{
/* update_map_draw_list */	MapV41DHUpdateDrawList,
/* draw_map_list        */      MapV41DHDrawMapList
	}
};

NhlClass NhlmapV41DataHandlerClass = (NhlClass)&NhlmapV41DataHandlerClassRec;

static int Level;
static int Point_Count;
static int VStrip;
static int Color,Dash_Pattern;
static float Dash_SegLen,Thickness;

static char *BGroup_Names[] = { 
	NhlmpNULLAREA,
	NhlmpALLNATIONAL,
	NhlmpALLGEOPHYSICAL,
	NhlmpLAND,
	NhlmpWATER,
	NhlmpINLANDWATER,
	NhlmpOCEANS,
	NhlmpCONTINENTS,
	NhlmpISLANDS,
	NhlmpLARGEISLANDS,
	NhlmpSMALLISLANDS,
	NhlmpALLUSSTATES,
	NhlmpUSSTATESLAND,
	NhlmpUSSTATESWATER };

static NrmQuark Qstring = NrmNULLQUARK;
static NrmQuark Qarea_names = NrmNULLQUARK;
static NrmQuark Qarea_types = NrmNULLQUARK;
static NrmQuark Qdynamic_groups = NrmNULLQUARK;
static NrmQuark Qfixed_groups = NrmNULLQUARK;

static NhlBoolean Part_Outside_Viewspace = False;
static NhlLayer Last_Instance = NULL;
static NhlMapPlotLayer Mpl;
static NhlMapPlotLayerPart *Mpp;
static NhlMapV41DataHandlerClassPart	*Mv41cp;
static NhlBoolean Amap_Inited;
static NhlBoolean Grid_Setup;
static mpDrawOp Draw_Op;

static mpDrawIdRec *DrawIds = NULL;

static int UsIds[3];
static int UsIdCount;
static int LandId,WaterId,OceanId;

static void mpLowerCase(char *string)
{
	char *cp = string;

	while (*cp != '\0') {
                *cp = tolower(*cp);
		cp++;
	}
}

/*
 * The original string is not modified, but copied into a static output
 * buffer. Therefore you can only work on one string at a time.
 * Either ':' or '.' indicate parent-child boundaries in the spec string.
 * To simplify the search, change '.' to ':'. Also in order to allow
 * some compatibility with the 4.0 database, '-' used as a word separator is
 * replaced with a space. However, we must be careful not to replace '-'
 * when it is part of the 4.1 name. In this case the '-' should always be
 * preceded and followed by a space (except possibly when a wild card is
 * involved, but this is not currently handled).
 */

static char *PrepareSpecString(char *string)
{
        static char outbuf[256];
	char *cp = string;
        NhlBoolean last_space = False;

        strcpy(outbuf,string);
        cp = outbuf;
	while (*cp != '\0') {
                switch (*cp) {
                    case '.':
                            *cp = ':';
                            break;
                    case ' ':
                            last_space = True;
                            break;
                    case '-':
                            if (! last_space)
                                    *cp = ' ';
                            break;
                    default:
                            last_space = False;
                            *cp = tolower(*cp);
                            break;
                }
		cp++;
	}
        return outbuf;
}

static NhlErrorTypes
MapV41DHClassPartInit
#if	NhlNeedProto
(
	NhlClass	lc
)
#else
(lc)
	NhlClass	lc;
#endif
{
	NhlMapV41DataHandlerClass	mdhc = (NhlMapV41DataHandlerClass)lc;
	NhlErrorTypes		ret = NhlNOERROR;
        NhlString		entry_name = "MapV41DHClassPartInit";
        
	Qstring = NrmStringToQuark(NhlTString);
	Qarea_names = NrmStringToQuark(NhlNmpAreaNames);
	Qarea_types = NrmStringToQuark(NhlNmpAreaTypes);
	Qdynamic_groups = NrmStringToQuark(NhlNmpDynamicAreaGroups);
	Qfixed_groups = NrmStringToQuark(NhlNmpFixedAreaGroups);
        
        Mv41cp = &mdhc->mapv41dh_class;
        Mv41cp->entity_rec_count = 0;
        Mv41cp->entity_recs = NULL;

	load_hlumap_routines(False);
        
	return NhlNOERROR;
}

static int alpha_sort
(
        const void *p1,
        const void *p2
)
{
        v41EntityRec *erec1 = *(v41EntityRec **) p1;
        v41EntityRec *erec2 = *(v41EntityRec **) p2;
        int ret;

        ret =  strcmp(erec1->name,erec2->name);
        if (! ret) {
                int ix1 = erec1->eid;
                int ix2 = erec2->eid;
                while (! ret) {
                        ix1 = c_mpipar(ix1);
                        ix2 = c_mpipar(ix2);
			if (! (ix1 && ix2))
				return 0;
                        ret = strcmp(Mv41cp->entity_recs[ix1-1].name,
                                     Mv41cp->entity_recs[ix2-1].name);
                }
        }
        return ret;
}

static NhlErrorTypes Init_Entity_Recs
#if	NhlNeedProto
(
	NhlMapV41DataHandlerLayer	mv41l,
        NhlString			entry_name
)
#else
(mv41l,entry_name)
	NhlMapV41DataHandlerLayer	mv41l;
        NhlString			entry_name;
#endif
{
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	char *e_text;
        int i,j,us_ix=0;
        int us_child_count[3] = {0,0,0};

            /* since the entity name list is dynamic, for now just get
               the entity identifier count (stored in entity_rec_count) */

        if (Mv41cp->entity_rec_count > 0) {
                mv41p->outline_rec_count = Mv41cp->entity_rec_count;
        }
        else {
                c_mplnri("Earth..1");
        
                for (i = 1; ;i++) {
                        int type = NGCALLF(mpiaty,MPIATY)(&i);
                        if (type == 0)
                                break;
                        if (!strcmp(c_mpname(i),"United States")) {
#if 0
                                printf("us id %d\n",i);
#endif
                                UsIds[us_ix++] = i;
                        }
                        if (!strcmp(c_mpname(i),"Land")) {
#if 0
                                printf("land id %d\n",i);
#endif
                                LandId = i;
                        }
                        if (!strcmp(c_mpname(i),"Water")) {
#if 0
                                printf("water id %d\n",i);
#endif
                                WaterId = i;
                        }
                        if (!strcmp(c_mpname(i),"Ocean")) {
#if 0
                                printf("ocean id %d\n",i);
#endif
                                OceanId = i;
                        }
                }
                UsIdCount = us_ix;
                Mv41cp->entity_rec_count = mv41p->outline_rec_count = i-1;
        }
        Mv41cp->entity_recs = NhlMalloc
                (sizeof(v41EntityRec) * Mv41cp->entity_rec_count);
        Mv41cp->alpha_recs = NhlMalloc
                (sizeof(v41EntityRec *) * Mv41cp->entity_rec_count);
        if (! Mv41cp->entity_recs || ! Mv41cp->alpha_recs) {
                e_text = "%s: dynamic memory allocation error";
                NhlPError(NhlFATAL,ENOMEM,e_text,entry_name);
                return NhlFATAL;
        }
        for (i = 0; i < Mv41cp->entity_rec_count; i++) {
                v41EntityRec *erec = &Mv41cp->entity_recs[i];
                char *buf;
                int j,pix[32],pcount,ix = i + 1;
                int type,lasttype = -1;
                
                Mv41cp->alpha_recs[i] = erec;
                erec->level = (int) NGCALLF(mpiaty,MPIATY)(&ix);
                erec->eid = ix;
                buf = c_mpname(ix);
                erec->name = NhlMalloc(strlen(buf)+1);
                if (!erec->name) {
                        e_text = "%s: dynamic memory allocation error";
                        NhlPError(NhlFATAL,ENOMEM,e_text,entry_name);
                        return NhlFATAL;
                }
                strcpy(erec->name,buf);
                mpLowerCase(erec->name);
                
                if (NGCALLF(mpipai,MPIPAI)(&ix,&LandId)) {
                        erec->fixed_gid = NhlmpLANDGROUPINDEX;
                        erec->dynamic_gid = NGCALLF(mpisci,MPISCI)(&ix) + 2;
                }
                else if (ix == OceanId || ix == WaterId)
                        erec->dynamic_gid = erec->fixed_gid
                                = NhlmpOCEANGROUPINDEX;
                else 
                        erec->dynamic_gid = erec->fixed_gid
                                = NhlmpINLANDWATERGROUPINDEX;
#if 0                
                buf = c_mpfnme(ix,1);
                printf("%d	%d	%d	%d	%s\n",
                       ix,erec->level,erec->fixed_gid,erec->dynamic_gid,buf);
                pix[0] = ix;
                for (j = 0; pix[j] != 0; j++)
                        pix[j+1] = c_mpipar(pix[j]);
                pcount = j;
                
                for (j = pcount-1; j >=0; j--) {
                        type = c_mpiaty(pix[j]);
                        if (lasttype > -1) {
                                if (type != lasttype)
                                        printf(" - ");
                                else
                                        printf(" . ");
                        }
                        buf = c_mpname(pix[j]);
                        printf("%s",buf);
                        lasttype = type;
                }
                printf("\n");
#endif
        }
        qsort(Mv41cp->alpha_recs,Mv41cp->entity_rec_count,
              sizeof(v41EntityRec *),alpha_sort);

        for (i = 0; i < Mv41cp->entity_rec_count; i++) {
                char buf[256];
                Mv41cp->alpha_recs[i]->unique = True;
                if (i < Mv41cp->entity_rec_count - 1) {
                        if (! strcmp(Mv41cp->alpha_recs[i]->name,
                              Mv41cp->alpha_recs[i+1]->name))
                                Mv41cp->alpha_recs[i]->unique = False;
                }
                if (i > 0) {
                        if (! strcmp(Mv41cp->alpha_recs[i]->name,
                              Mv41cp->alpha_recs[i-1]->name))
                                Mv41cp->alpha_recs[i]->unique = False;
                }
#if 0                
                if (Mv41cp->alpha_recs[i]->unique) {
                        strcpy(buf,Mv41cp->alpha_recs[i]->name);
                }
                else {
                        int pix = c_mpipar(Mv41cp->alpha_recs[i]->eid);
                        int ptype = c_mpiaty(pix);
                        int etype = c_mpiaty(Mv41cp->alpha_recs[i]->eid);
                        
                        strcpy(buf,Mv41cp->entity_recs[pix-1].name);
                        if (ptype == etype)
                                strcat(buf," . ");
                        else 
                                strcat(buf," - ");
                        strcat(buf,Mv41cp->alpha_recs[i]->name);
                }
                printf("%s\n",buf);
#endif
        }
        
#if 0        
        for (i = 1; i <= Mv41cp->entity_rec_count; i++) {
                if (NGCALLF(mpipai,MPIPAI)(&i,&UsIds[0])) {
                        printf("%d -- child type %d name %s\n",UsIds[0],
                               NGCALLF(mpiaty,MPIATY)(&i),
                               c_mpname(i));
                        us_child_count[0]++;
                }
                if (NGCALLF(mpipai,MPIPAI)(&i,&UsIds[1])) {
                        printf("%d -- child type %d name %s\n",UsIds[1],
                               NGCALLF(mpiaty,MPIATY)(&i),
                               c_mpname(i));
                        us_child_count[1]++;
                }
                if (NGCALLF(mpipai,MPIPAI)(&i,&UsIds[2])) {
                        printf("%d -- child type %d name %s\n",UsIds[2],
                               NGCALLF(mpiaty,MPIATY)(&i),
                               c_mpname(i));
                        us_child_count[2]++;
                }
        }
        printf("%s: %d children, %s: %d children, %s: %d children\n",
               c_mpname(UsIds[0]),us_child_count[0],
               c_mpname(UsIds[1]),us_child_count[1],
               c_mpname(UsIds[2]),us_child_count[2]);
#endif                
        
	return NhlNOERROR;
}    
        

/*
 * Function:  mdhManageDynamicArrays
 *
 * Description: Creates and manages internal copies of each of the 
 *	MapDataHandler GenArrays. 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    mdhManageDynamicArrays
#if	NhlNeedProto
(
	NhlMapV41DataHandlerLayer	mv4new, 
	NhlMapV41DataHandlerLayer	mv4old,
	NhlBoolean			init,
	_NhlArgList			args,
	int				num_args)
#else
(mdhnew,mdhold,init,args,num_args)
	NhlMapV41DataHandlerLayer	mv4new;
	NhlMapV41DataHandlerLayer	mv4old;
	NhlBoolean			init;
	_NhlArgList			args;
	int				num_args;
#endif

{
	NhlMapDataHandlerLayerPart	*mdhp = &(mv4new->mapdh);
	NhlMapDataHandlerLayerPart	*omdhp = &(mv4old->mapdh);
        
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	char *entry_name;
	char *e_text;
	NhlGenArray ga;
	int i,count;
	int *ip;
	NhlString *sp;
	NhlBoolean need_check;
	NhlBoolean use_default;
        int entity_rec_count;
        v41EntityRec *entity_recs;

	entry_name =  init ?
                "MapDataHandlerInitialize" : "MapDataHandlerSetValues";

        entity_rec_count = Mv41cp->entity_rec_count;
        entity_recs = Mv41cp->entity_recs;
        
/*
 * Area Name specifiers
 */
	ga = init ? NULL : omdhp->area_names;

	if (ga != mdhp->area_names) {
		if (! mdhp->area_names ||
                    mdhp->area_names->num_elements != entity_rec_count) {
			e_text = 
			  "%s: %s GenArray must contain %d elements: ignoring";
			NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
				  NhlNmpAreaNames,entity_rec_count);
			ret = MIN(NhlWARNING,ret);
			mdhp->area_names = ga;
		}
		else {
			NhlFreeGenArray(ga);
			if ((ga = _NhlCopyGenArray(mdhp->area_names,
						   True)) == NULL) {
				e_text = "%s: error copying %s GenArray";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name,
					  NhlNmpAreaNames);
				return NhlFATAL;
			}
			omdhp->area_names = NULL;
			mdhp->area_names = ga;
			/* Check elements for null strings */
			sp = (NhlString *) mdhp->area_names->data;
			for (i = 0; i < mdhp->area_names->num_elements; i++) {
				if (sp[i] == NULL || strlen(sp[i]) == 0) {
                                        char *buf = c_mpname(i+1);
					e_text = 
		 "%s: Null or zero length %s string for index %d: defaulting";
					NhlPError(NhlWARNING,NhlEUNKNOWN,
						  e_text,entry_name,
						  NhlNmpAreaNames,i);
					ret = MIN(ret,NhlWARNING);
					if (sp[i] != NULL) NhlFree(sp[i]);
                                        
					sp[i] = NhlMalloc(strlen(buf) + 1);
					if (sp[i] == NULL) {
						e_text = 
				       "%s: dynamic memory allocation error";
						NhlPError(NhlFATAL,ENOMEM,
							  e_text,entry_name);
						return NhlFATAL;
					}
					strcpy(sp[i],buf);
				}
			}
		}
	}

/*
 * The dynamic area Groups
 */
	ga = init ? NULL : omdhp->dynamic_groups;

	need_check = False;
	if (ga != mdhp->dynamic_groups) {
		if (! mdhp->dynamic_groups ||
                    mdhp->dynamic_groups->num_elements != entity_rec_count) {
			e_text = 
			  "%s: %s GenArray must contain %d elements: ignoring";
			NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
				  NhlNmpDynamicAreaGroups,entity_rec_count);
			ret = MIN(NhlWARNING,ret);
		}
		else {
			NhlFreeGenArray(ga);
			if ((ga = _NhlCopyGenArray(mdhp->dynamic_groups,
 						   True)) == NULL) {
				e_text = "%s: error copying %s GenArray";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name,
					  NhlNmpAreaNames);
				return NhlFATAL;
			}
			need_check = True;
			omdhp->dynamic_groups = NULL;
		}
		mdhp->dynamic_groups = ga;

	}
	if (need_check || mdhp->area_group_count < omdhp->area_group_count) {
		ip = (int *) ga->data;
		for (i=0; i < ga->num_elements; i++) {
			use_default = False;
			if (ip[i] < NhlmpOCEANGROUPINDEX)
				use_default = True;
			else if (ip[i] > mdhp->area_group_count - 1) {
				e_text =
	         "%s: %s index %d holds an invalid fill group id: defaulting";
				NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
					  entry_name,
					  NhlNmpDynamicAreaGroups,i);
				ret = MIN(ret, NhlWARNING);
				use_default = True;
			}
			if (use_default)
				ip[i] = entity_recs[i].dynamic_gid;
		}
	}

/*
 * The fixed area groups are read only
 */
	ga = init ? NULL : omdhp->fixed_groups;

	if (ga != mdhp->fixed_groups) {
		e_text = "%s: attempt to set read-only resource %s ignored";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
			  NhlNmpFixedAreaGroups);
		ret = MIN(ret, NhlWARNING);
		mdhp->fixed_groups = ga;
	}


/*
 * Area types are read only
 */
	ga = init ? NULL : omdhp->area_types;

	if (ga != mdhp->area_types) {
		e_text = "%s: attempt to set read-only resource %s ignored";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
			  NhlNmpAreaTypes);
		ret = MIN(ret, NhlWARNING);
		mdhp->area_types = ga;
	}

	return ret;
}

/*
 * Function:	MapV41DHInitialize
 *
 * Description: 
 *
 * In Args: 	class	objects layer_class
 *		req	instance record of requested values
 *		new	instance record of new object
 *		args	list of resources and values for reference
 *		num_args 	number of elements in args.
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes
MapV41DHInitialize
#if	NhlNeedProto
(
	NhlClass	class,
	NhlLayer	req,
	NhlLayer	new,
	_NhlArgList	args,
	int		num_args
)
#else
(class,req,new,args,num_args)
        NhlClass   class;
        NhlLayer        req;
        NhlLayer        new;
        _NhlArgList     args;
        int             num_args;
#endif
{
        NhlMapV41DataHandlerLayer mv41l = (NhlMapV41DataHandlerLayer) new;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = "MapV41DHInitialize";
	char			*e_text;

        
	mv41p->aws_id = -1;
	mv41p->new_amap_req = True;
	mv41p->fill_rec_alloc = 0;
	mv41p->fill_rec_count = 0;
	mv41p->fill_recs = NULL;
	mv41p->outline_rec_alloc = 0;
	mv41p->outline_rec_count = 0;
	mv41p->outline_recs = NULL;

        ret = Init_Entity_Recs(mv41l,entry_name);
        if (ret < NhlWARNING) {
                char e_text[] = "%s: error initializing map outline records";
                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
                return(ret);
        }
        
/* Manage the dynamic arrays */

	subret = mdhManageDynamicArrays((NhlMapV41DataHandlerLayer)new,
                                        (NhlMapV41DataHandlerLayer)req,
                                        True,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing dynamic arrays";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

        return ret;
}

/*
 * Function:	MapV41DHSetValues
 *
 * Description: 
 *
 * In Args:	old	copy of old instance record
 *		reference	requested instance record
 *		new	new instance record	
 *		args 	list of resources and values for reference
 *		num_args	number of elements in args.
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes MapV41DHSetValues
#if	NhlNeedProto
(
	NhlLayer	old,
	NhlLayer	reference,
	NhlLayer	new,
	_NhlArgList	args,
	int		num_args
)
#else
(old,reference,new,args,num_args)
	NhlLayer	old;
	NhlLayer	reference;
	NhlLayer	new;
	_NhlArgList	args;
	int		num_args;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = "MapV41DHSetValues";
	char			*e_text;

/* Manage the dynamic arrays */

	subret = mdhManageDynamicArrays((NhlMapV41DataHandlerLayer)new,
                                        (NhlMapV41DataHandlerLayer)old,
                                        False,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing dynamic arrays";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

        return ret;
}

/*
 * Function:	mdhGetNewGenArray
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlGenArray mdhGetNewGenArray
#if	NhlNeedProto
(
 	NhlMapV41DataHandlerLayer       mv41l,
	NrmQuark			quark,
        NhlString			entry_name
)
#else
(mv41l,quark,entry_name)
	NhlMapV41DataHandlerLayer       mv41l;
	NrmQuark			quark;
        NhlString			entry_name;
#endif
{
	char *e_text;
	int i, len;
	NhlGenArray ga;
        int entity_rec_count;
        v41EntityRec *entity_recs;

        entity_rec_count = Mv41cp->entity_rec_count;
        entity_recs = Mv41cp->entity_recs;
        
	if (quark == Qarea_names) {
		NhlString	*sp;
		len = entity_rec_count;
                
		if ((sp = NhlMalloc(sizeof(NhlString)*len)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,
				  NhlEUNKNOWN,e_text,entry_name);
			return NULL;
		}
		for (i = 0; i < len; i++) {
                        char *buf = c_mpname(i+1);
			if ((sp[i] = NhlMalloc
                             (strlen(buf) + 1)) == NULL) {
				e_text = "%s: dynamic memory allocation error";
				NhlPError(NhlFATAL,NhlEUNKNOWN,
					  e_text,entry_name);
				return NULL;
			}
			strcpy(sp[i],buf);
		}
		if ((ga = NhlCreateGenArray(sp,NhlTString,sizeof(NhlString),
					    1,&len)) == NULL) {
			e_text = "%s: error creating gen array";
			NhlPError(NhlFATAL,NhlEUNKNOWN,
				  e_text,entry_name);
			return NULL;
		}
		return ga;
	}
	else if (quark == Qarea_types) {
		int	*ip;
		len = entity_rec_count;
		if ((ip = NhlMalloc(sizeof(int)*len)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,
				  NhlEUNKNOWN,e_text,"MapPlotGetValues");
			return NULL;
		}
		for (i = 0; i < len; i++) {
			ip[i] = entity_recs[i].level;
		}
		if ((ga = NhlCreateGenArray(ip,NhlTInteger,sizeof(int),
					    1,&len)) == NULL) {
			e_text = "%s: error creating gen array";
			NhlPError(NhlFATAL,NhlEUNKNOWN,
				  e_text,entry_name);
			return NULL;
		}
		return ga;

	}
        else if (quark == Qfixed_groups) {
		int	*ip;
		len = entity_rec_count;
		if ((ip = NhlMalloc(sizeof(int)*len)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,
				  NhlEUNKNOWN,e_text,"MapPlotGetValues");
			return NULL;
		}
		for (i = 0; i < len; i++) {
                        ip[i] = entity_recs[i].fixed_gid;
		}
		if ((ga = NhlCreateGenArray(ip,NhlTInteger,sizeof(int),
					    1,&len)) == NULL) {
			e_text = "%s: error creating gen array";
			NhlPError(NhlFATAL,NhlEUNKNOWN,
				  e_text,entry_name);
			return NULL;
		}
		return ga;
        }
	else if (quark == Qdynamic_groups) {
		int	*ip;
		int	index = quark == Qdynamic_groups ? 1 : 0;

		len = entity_rec_count;
		if ((ip = NhlMalloc(sizeof(int)*len)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,
				  NhlEUNKNOWN,e_text,"MapPlotGetValues");
			return NULL;
		}
		for (i = 0; i < len; i++) {
                        ip[i] = entity_recs[i].dynamic_gid;
		}
		if ((ga = NhlCreateGenArray(ip,NhlTInteger,sizeof(int),
					    1,&len)) == NULL) {
			e_text = "%s: error creating gen array";
			NhlPError(NhlFATAL,NhlEUNKNOWN,
				  e_text,"MapPlotGetValues");
			return NULL;
		}
		return ga;
	}
	return NULL;
}

/*
 * Function:  mpGenArraySubsetCopy
 *
 * Description: Since the internal GenArrays maintained by the Contour object
 *      may be bigger than the size currently in use, this function allows
 *      a copy of only a portion of the array to be created. This is for
 *      use by the GetValues routine when returning GenArray resources to
 *      the user level. The array is assumed to be valid. The only pointer
 *      type arrays that the routine can handle are NhlString arrays.
 *      Note: this might be another candidate for inclusion as a global
 *      routine.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */

static NhlGenArray mdhGenArraySubsetCopy
#if	NhlNeedProto
        (NhlGenArray    ga,
        int             length)
#else
(ga,length)
        NhlGenArray     ga;
        int             length;
#endif
{
        NhlGenArray gto;

        if (length > ga->num_elements)
                return NULL;

        if ((gto = _NhlCopyGenArray(ga,False)) == NULL) {
                return NULL;
        }
        if ((gto->data = (NhlPointer) NhlMalloc(length * ga->size)) == NULL) {
                return NULL;
        }
        if (ga->typeQ != Qstring) {
                memcpy((void *)gto->data, (Const void *) ga->data,
                       length * ga->size);
        }
        else {
                NhlString *cfrom = (NhlString *) ga->data;
                NhlString *cto = (NhlString *) gto->data;
                int i;
                for (i=0; i<length; i++) {
                        if ((*cto = (char *)
                             NhlMalloc(strlen(*cfrom)+1)) == NULL) {
                                return NULL;
                        }
                        strcpy(*cto++,*cfrom++);
                }
        }
        gto->num_elements = length;
	gto->my_data = True;
        return gto;
}

/*
 * Function:    MapV41DHGetValues
 *
 * Description: Retrieves the current setting of MapV41DataHandler resources.
 *      Actually the resources belong to the superclass MapDataHandler --
 *      but they get their contents from the subclass.
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 *      Memory is allocated when any of the following resources are retrieved:
 *		NhlNmpAreaNames
 *		NhlNmpAreaTypes
 *		NhlNmpDynamicAreaGroups
 *		NhlNmpSpecifiedFillColors
 *
 *      The caller is responsible for freeing this memory.
 */

static NhlErrorTypes    MapV41DHGetValues
#if	NhlNeedProto
(NhlLayer l, _NhlArgList args, int num_args)
#else
(l,args,num_args)
        NhlLayer        l;
        _NhlArgList     args;
        int     	num_args;
#endif
{
        NhlMapV41DataHandlerLayer mv41l = (NhlMapV41DataHandlerLayer) l;
        NhlMapDataHandlerLayerPart *mdhp = &mv41l->mapdh;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
        NhlGenArray ga;
        NhlString e_text,entry_name = "MapV41DHGetValues";
        int i, count = 0;
	NhlBoolean create_it;

        for (i = 0; i < num_args; i++ ) {

		create_it = False;
                ga = NULL;
                
                if (args[i].quark == Qarea_names) {
			create_it = True;
                        ga = mdhp->area_names;
                        count = ga ? ga->num_elements : 0;
                }
                else if (args[i].quark == Qarea_types) {
			create_it = True;
                        ga = mdhp->area_types;
                        count = ga ? ga->num_elements : 0;
                }
                else if (args[i].quark == Qdynamic_groups) {
			create_it = True;
                        ga = mdhp->dynamic_groups;
                        count = ga ? ga->num_elements : 0;
                }
                else if (args[i].quark == Qfixed_groups) {
			create_it = True;
                        ga = mdhp->fixed_groups;
                        count = ga ? ga->num_elements : 0;
                }
                if (ga != NULL) {
                        if ((ga = mdhGenArraySubsetCopy(ga, count)) == NULL) {
                                e_text = "%s: error copying %s GenArray";
                                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
                                          entry_name,
					  NrmQuarkToString(args[i].quark));
                                return NhlFATAL;
                        }
                        *((NhlGenArray *)(args[i].value.ptrval)) = ga;
                }
		else if (create_it) {
			if ((ga = mdhGetNewGenArray(mv41l,args[i].quark,
                                                    entry_name)) 
			    == NULL) {
				e_text = "%s: error getting %s GenArray";
                                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
                                          entry_name,
					  NrmQuarkToString(args[i].quark));
                                return NhlFATAL;
                        }
                        *((NhlGenArray *)(args[i].value.ptrval)) = ga;

		}
        }

        return(NhlNOERROR);

}


/*
 * Function:    MapV41DHDestroy
 *
 * Description: Retrieves the current setting of MapV41DataHandler resources.
 *      Actually the resources belong to the superclass MapDataHandler --
 *      but they get their contents from the subclass.
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 *      Memory is allocated when any of the following resources are retrieved:
 *		NhlNmpAreaNames
 *		NhlNmpAreaTypes
 *		NhlNmpDynamicAreaGroups
 *		NhlNmpSpecifiedFillColors
 *
 *      The caller is responsible for freeing this memory.
 */

static NhlErrorTypes    MapV41DHDestroy
#if	NhlNeedProto
(
        NhlLayer l
        )
#else
(l)
        NhlLayer        l;
#endif
{
        NhlMapV41DataHandlerLayer mv41l = (NhlMapV41DataHandlerLayer) l;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;

	if (mv41p->fill_recs != NULL)
		NhlFree(mv41p->fill_recs);
	if (mv41p->outline_recs != NULL)
		NhlFree(mv41p->outline_recs);

        return NhlNOERROR;
}

/*
 * Routines called by MapV41DHUpdateDrawList to set up the list of areas
 * to be drawn -- there are two lists, one for fill and masking, the other
 * for outlines.
 */

static int fill_sort
(
        const void *p1,
        const void *p2
)
{
        v41SpecFillRec frec1 = *(v41SpecFillRec *) p1;
        v41SpecFillRec frec2 = *(v41SpecFillRec *) p2;
        int lev1,lev2;
        int pix;
        int ret = 0;
/*
 * The draw id records are set up based on the specified fill records -
 * Later entries in the list modify the list last, and therefore override
 * earlier entries. So higher priority items are moved to the end of the
 * list.
 */

/*
 * sort first by level
 */
        lev1 = c_mpiaty(frec1.eid);
        lev2 = c_mpiaty(frec2.eid);
        if (lev1 != lev2)
                return (lev1 - lev2);
/*
 * next by parent child relationships within the same level
 */
        pix = frec1.eid;
        while (pix) {
                pix = c_mpipar(pix);
                if (pix == frec2.eid)
                        return 1;
        }
        pix = frec2.eid;
        while (pix) {
                pix = c_mpipar(pix);
                if (pix == frec1.eid)
                        return -1;
        }
/*
 * next sort by draw mode, masking follows fill, causing it to override
 */
        if (frec1.draw_mode != frec2.draw_mode)
                return frec1.draw_mode - frec2.draw_mode;

/*
 * Finally sort by entity id (not really necessary)
 */
        return (frec1.eid - frec2.eid);
}

static char *UpNameHierarchy
#if	NhlNeedProto
(
        char	*string
        )
#else
(string)
	char	*string;
#endif
{
        char *cp,*bcp;
        int i;

        cp = string;
        
        cp = strrchr(cp,':');
        if (cp > string) {
                *cp = '\0';
                bcp = cp - 1;
                while (isspace(*bcp))
                        *(bcp--) = '\0';
                cp++;
        }
        else
                cp = string;
        
        while (isspace(*cp))
                cp++;

        return cp;
}


/*
 * Function:  UpdateSpecFillRecords
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    UpdateSpecFillRecords
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer	mv41l,
	NhlMapPlotLayerPart		*mpp,
        unsigned char			draw_mode,
        NhlString			spec_string,
        int				spec_fill_index,
        NhlString			entry_name)
#else
(mv41l,mpp,draw_mode,spec_string,spec_fill_index,entry_name)
	NhlMapV41DataHandlerLayer 	mv41l;
	NhlMapPlotLayerPart		*mpp;
        unsigned char			draw_mode;
        NhlString			spec_string;
        int				spec_fill_index;
        NhlString			entry_name;
#endif
{
        NhlMapDataHandlerLayerPart *mdhp = &mv41l->mapdh;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
        char *comp_string, *parent_string = NULL;
        char *e_text;
        int i;
                
        comp_string = UpNameHierarchy(spec_string);
        for (i = 0; i < Mv41cp->entity_rec_count; i++) {
                int count = mv41p->fill_rec_count;
                NhlBoolean unique;
                
                if (strcmp(comp_string,Mv41cp->alpha_recs[i]->name))
                        continue;
                
                unique = Mv41cp->alpha_recs[i]->unique;
                if (! unique) {
                            /* see if a parent has been specified to
                             uniquely qualify the name */
                        
                        if (! parent_string)
                                parent_string = UpNameHierarchy(spec_string);
                        if (parent_string != comp_string) {
                                int peid;
                                peid = c_mpipar(Mv41cp->alpha_recs[i]->eid);
                                if (strcmp(parent_string,
                                           Mv41cp->entity_recs[peid-1].name))
                                        continue;
                                unique = True;
                        }
                }

                if (count == mv41p->fill_rec_alloc) {
                        mv41p->fill_recs = NhlMalloc
                                (sizeof(v41SpecFillRec) * v41ALLOC_UNIT);
                        if (! mv41p->fill_recs) {
                                e_text = "%s: dynamic memory allocation error";
                                NhlPError(NhlFATAL,ENOMEM,e_text,entry_name);
                                return NhlFATAL;
                        }
                        mv41p->fill_rec_alloc += v41ALLOC_UNIT;
                }
                mv41p->fill_recs[count].eid = Mv41cp->alpha_recs[i]->eid;
                mv41p->fill_recs[count].spec_ix = spec_fill_index;
                mv41p->fill_recs[count].draw_mode = draw_mode;
                mv41p->fill_recs[count].spec_col = 0;
                mv41p->fill_recs[count].spec_pat = 0;
                mv41p->fill_recs[count].spec_fscale = 0;
                if (mpp->spec_fill_color_count > spec_fill_index) {
			int *ip = (int *) mpp->spec_fill_colors->data;
			if (ip[spec_fill_index] != NhlmpUNSETCOLOR)
				mv41p->fill_recs[count].spec_col =  1;
		}
		if (mpp->spec_fill_pattern_count > spec_fill_index) {
			int *ip = (int *) mpp->spec_fill_patterns->data;
			if (ip[spec_fill_index] != NhlmpUNSETFILLPATTERN)
				mv41p->fill_recs[count].spec_pat =  1;
		}
		if (mpp->spec_fill_scale_count > spec_fill_index) {
			float *fp = (float *) mpp->spec_fill_scales->data;
			if (fp[spec_fill_index] > NhlmpUNSETFILLSCALE)
				mv41p->fill_recs[count].spec_fscale =  1;
		}
                mv41p->fill_rec_count++;
                
                if (unique)
                        break;
                    /* Otherwise add all matching items */
        }
        return ret;
        
}

/*
 * Function:  mv41BuildFillDrawList
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */


/*ARGSUSED*/
static NhlErrorTypes    mv41BuildFillDrawList
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer	mv41l,
	NhlMapPlotLayerPart		*mpp,
        NhlString			entry_name)
#else
(mv41l,mpp,entry_name)
	NhlMapV41DataHandlerLayer 	mv41l;
	NhlMapPlotLayerPart		*mpp;
        NhlString			entry_name;
#endif
{
        NhlMapDataHandlerLayerPart *mdhp = &mv41l->mapdh;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	int			i;
	NhlString		*sp;
        
        mv41p->fill_rec_count = 0;
        mv41p->min_fill_level = 0;
        
	if (mpp->fill_area_specs != NULL) {
		sp = (NhlString *) mpp->fill_area_specs->data;
		for (i = 0; i < mpp->fill_area_specs->num_elements; i++) {
                        char *spec_string = PrepareSpecString(sp[i]);
                        subret = UpdateSpecFillRecords
                                (mv41l,mpp,mpDRAW,spec_string,i,entry_name);
			if ((ret = MIN(ret,subret)) < NhlWARNING)
				return ret;

		}
	}
	if (mpp->area_masking_on && mpp->mask_area_specs != NULL) {
		sp = (NhlString *) mpp->mask_area_specs->data;
		for (i = 0; i < mpp->mask_area_specs->num_elements; i++) {
			char *spec_string = PrepareSpecString(sp[i]);
                        subret = UpdateSpecFillRecords
                                (mv41l,mpp,mpMASK,spec_string,i,entry_name);
			if ((ret = MIN(ret,subret)) < NhlWARNING)
				return ret;
		}
	}
        if (mv41p->fill_rec_count) {
                qsort(mv41p->fill_recs,mv41p->fill_rec_count,
                      sizeof(v41SpecFillRec),fill_sort);
        
                mv41p->min_fill_level =
                        MAX(c_mpiaty
                            (mv41p->fill_recs[mv41p->fill_rec_count-1].eid),
                            mv41p->min_fill_level);
        }
        
        return ret;
        
}

static int outline_sort
(
        const void *p1,
        const void *p2
)
{
        v41SpecLineRec lrec1 = *(v41SpecLineRec *) p1;
        v41SpecLineRec lrec2 = *(v41SpecLineRec *) p2;
        int lev1,lev2;
        int pix;
        int ret = 0;
/*
 * The draw id records are set up based on the specified outline records -
 * Later entries in the list modify the list last, and therefore override
 * earlier entries. So higher priority items are moved to the end of the
 * list.
 */
/*
 * sort first by level
 */
        lev1 = c_mpiaty(lrec1.eid);
        lev2 = c_mpiaty(lrec2.eid);
        if (lev1 != lev2)
                return (lev1 - lev2);
/*
 * next by parent child relationships within the same level
 */
        pix = lrec1.eid;
        while (pix) {
                pix = c_mpipar(pix);
                if (pix == lrec2.eid)
                        return 1;
        }
        pix = lrec2.eid;
        while (pix) {
                pix = c_mpipar(pix);
                if (pix == lrec1.eid)
                        return -1;
        }

/*
 * Finally sort by entity id (not really necessary)
 */
        return (lrec1.eid - lrec2.eid);
}
 
/*
 * Function:  UpdateSpecLineRecords
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    UpdateSpecLineRecords
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer	mv41l,
	NhlMapPlotLayerPart		*mpp,
        unsigned char			draw_mode,
        NhlString			spec_string,
        int				spec_line_index,
        NhlString			entry_name)
#else
(mv41l,mpp,draw_mode,spec_string,spec_line_index,entry_name)
	NhlMapV41DataHandlerLayer 	mv41l;
	NhlMapPlotLayerPart		*mpp;
        unsigned char			draw_mode;
        NhlString			spec_string;
        int				spec_line_index;
        NhlString			entry_name;
#endif
{
        NhlMapDataHandlerLayerPart *mdhp = &mv41l->mapdh;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
        char *comp_string, *parent_string = NULL;
        char *e_text;
        int i;

        comp_string = UpNameHierarchy(spec_string);
        for (i = 0; i < Mv41cp->entity_rec_count; i++) {
                int count = mv41p->outline_rec_count;
                NhlBoolean unique;
                
                if (strcmp(comp_string,Mv41cp->alpha_recs[i]->name))
                        continue;
                
                unique = Mv41cp->alpha_recs[i]->unique;
                if (! unique) {
                            /* see if a parent has been specified to
                             uniquely qualify the name */
                        
                        if (! parent_string)
                                parent_string = UpNameHierarchy(spec_string);
                        if (parent_string != comp_string) {
                                int peid;
                                peid = c_mpipar(Mv41cp->alpha_recs[i]->eid);
                                if (strcmp(parent_string,
                                           Mv41cp->entity_recs[peid-1].name))
                                        continue;
                                unique = True;
                        }
                }
                
                if (count == mv41p->outline_rec_alloc) {
                        mv41p->outline_recs = NhlMalloc
                                (sizeof(v41SpecLineRec) * v41ALLOC_UNIT);
                        if (! mv41p->outline_recs) {
                                e_text = "%s: dynamic memory allocation error";
                                NhlPError(NhlFATAL,ENOMEM,e_text,entry_name);
                                return NhlFATAL;
                        }
                        mv41p->outline_rec_alloc += v41ALLOC_UNIT;
                }
                mv41p->outline_recs[count].eid = Mv41cp->alpha_recs[i]->eid;
                mv41p->outline_recs[count].spec_ix = spec_line_index;
                mv41p->outline_recs[count].spec_col = 0;
                mv41p->outline_recs[count].spec_dpat = 0;
                mv41p->outline_recs[count].spec_thickness = 0;
                mv41p->outline_rec_count++;
                
                if (unique)
                        break;
                    /* Otherwise add all matching items */
        }
        return ret;
        
}

/*
 * Function:  mv41BuildOutlineDrawList
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */


/*ARGSUSED*/
static NhlErrorTypes    mv41BuildOutlineDrawList
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer	mv41l,
	NhlMapPlotLayerPart		*mpp,
        NhlString			entry_name)
#else
(mv41l,mpp,entry_name)
	NhlMapV41DataHandlerLayer 	mv41l;
	NhlMapPlotLayerPart		*mpp;
        NhlString			entry_name;
#endif
{
        NhlMapDataHandlerLayerPart *mdhp = &mv41l->mapdh;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	int			i;
	NhlString		*sp;
        
        mv41p->outline_rec_count = 0;
        mv41p->min_outline_level = 0;
        
	if (mpp->outline_specs != NULL) {
		sp = (NhlString *) mpp->outline_specs->data;
		for (i = 0; i < mpp->outline_specs->num_elements; i++) {
			char *spec_string = PrepareSpecString(sp[i]);
                        subret = UpdateSpecLineRecords
                                (mv41l,mpp,mpDRAW,spec_string,i,entry_name);
			if ((ret = MIN(ret,subret)) < NhlWARNING)
				return ret;

		}
	}
        if (mv41p->outline_rec_count) {
                qsort(mv41p->outline_recs,mv41p->outline_rec_count,
                      sizeof(v41SpecLineRec),outline_sort);
        
                mv41p->min_outline_level =
                        MAX(c_mpiaty
                         (mv41p->outline_recs[mv41p->outline_rec_count-1].eid),
                            mv41p->min_outline_level);
        }
        
        return ret;
        
}

static NhlErrorTypes MapV41DHUpdateDrawList
#if	NhlNeedProto
(
	NhlLayer	instance,
        NhlBoolean  	init,
        NhlMapPlotLayer newmp,
        NhlMapPlotLayer oldmp,
        _NhlArgList	args,
        int             num_args
        )
#else
(instance,newmp,oldmp,args,num_args)
	NhlLayer	instance;
        NhlBoolean  	init;
        NhlMapPlotLayer newmp;
        NhlMapPlotLayer oldmp;
        _NhlArgList	args;
        int             num_args;
#endif
{
        NhlMapV41DataHandlerLayer mv41l = 
		(NhlMapV41DataHandlerLayer) instance;
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlMapPlotLayerPart	*mpp = &(newmp->mapplot);
	NhlString e_text, entry_name = "MapV41DHUpdateDrawList";
        NhlErrorTypes ret = NhlNOERROR,subret = NhlNOERROR;
        NhlBoolean build_fill_list = False, build_outline_list = False;
        
        
        if (init) {
               mv41p->new_amap_req = True;
               build_fill_list = True;
               build_outline_list = True;
        }
	else {
                NhlMapPlotLayerPart	*ompp = &(oldmp->mapplot);
                if (mpp->database_version != ompp->database_version ||
                    mpp->fill_boundaries != ompp->fill_boundaries ||
                    mpp->fill_area_specs != ompp->fill_area_specs ||
                    mpp->mask_area_specs != ompp->mask_area_specs ||
                    mpp->spec_fill_colors != ompp->spec_fill_colors ||
                    mpp->spec_fill_patterns != ompp->spec_fill_patterns ||
                    mpp->spec_fill_scales != ompp->spec_fill_scales ||
                    mpp->spec_fill_direct != ompp->spec_fill_direct ||
                    mpp->area_names != ompp->area_names ||
                    mpp->dynamic_groups != ompp->dynamic_groups ||
                    mpp->area_masking_on != ompp->area_masking_on ||
                    mpp->spec_fill_priority != ompp->spec_fill_priority)
                        build_fill_list = True;
                
                if (mpp->database_version != ompp->database_version ||
                    mpp->outline_boundaries != ompp->outline_boundaries ||
                    mpp->outline_specs != ompp->outline_specs)
                        build_outline_list = True;
                
                if (mpp->fill_on ||
                    (mpp->grid.on && mpp->grid_mask_mode != NhlMASKNONE)) {
                        if (build_fill_list || mpp->view_changed ||
                            mpp->trans_change_count !=
                            ompp->trans_change_count ||
                            mpp->fill_on != ompp->fill_on)
                                mv41p->new_amap_req = True;
                }
        }
        if (build_fill_list) {
                subret = mv41BuildFillDrawList(mv41l,mpp,entry_name);
                if ((ret = MIN(ret,subret)) < NhlWARNING)
                        return ret;
        }
        if (build_outline_list) {
                subret = mv41BuildOutlineDrawList(mv41l,mpp,entry_name);
                if ((ret = MIN(ret,subret)) < NhlWARNING)
                        return ret;
        }
        
                
        return ret;
}


/*
 * Routines called by MapV41DHDrawMapList to perform map fill, outline,
 * and grid using the Version 4.0 database.
 */

/*
 * Function:  hlumapfill
 *
 * Description: C version of APR user routine called from within ARSCAM 
 *		to fill areas based on the area ID.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static int (_NHLCALLF(hlumapfill,HLUMAPFILL))
#if	NhlNeedProto
(
	float *xcs, 
	float *ycs, 
	int *ncs, 
	int *iai, 
	int *iag, 
	int *nai
)
#else
(xcs,ycs,ncs,iai,iag,nai)
	float *xcs; 
	float *ycs; 
	int *ncs; 
	int *iai; 
	int *iag; 
	int *nai;
#endif
{
	int ix, pat_ix, col_ix, id;
	float fscale;
	unsigned char s_ix;
        int i,gid,geo_ix = -1 ,vs_ix = -1;

	if (Mpp == NULL) return 0;

        for (i = 0; i < *nai; i++) {
                if (iag[i] == 1) geo_ix = iai[i];
                if (iag[i] == 2) vs_ix = iai[i];
        }
        if (geo_ix > 0 && vs_ix > -1) {
                int ix,level = Level;
                int spec_col_ix = -1,spec_pat_ix = -1, spec_fscale_ix = -1;
                
		if (Mpp->fill_boundaries == NhlGEOPHYSICALANDUSSTATES ||
		    Mpp->fill_boundaries == NhlUSSTATES) {
                        NhlBoolean found = False;
                        for (i = UsIdCount-1;i>-1;i--) {
                                if (NGCALLF(mpipai,MPIPAI)(&geo_ix,&UsIds[i])){
                                        found = True;
                                        break;
                                }
                        }
                        if (! found)
				level = Mpp->fill_boundaries == NhlUSSTATES ?
					0 : 2;
                }
                if (DrawIds[geo_ix-1].spec_rec) {
                        v41SpecFillRec *fill_rec =
                                (v41SpecFillRec *)DrawIds[geo_ix-1].spec_rec;
                        if (fill_rec->draw_mode > mpDRAW)
                                return 0;
                        level = c_mpiaty(fill_rec->eid);
                        if (fill_rec->spec_col)
                                spec_col_ix = fill_rec->spec_ix;
                        if (fill_rec->spec_pat)
                                spec_pat_ix = fill_rec->spec_ix;
                        if (fill_rec->spec_fscale)
                                spec_fscale_ix = fill_rec->spec_ix;
                }
                
		/* determine the group id */

		if (level == 0)
			gid = 0;
		else {
                	if (level == 2 &&
                            ! DrawIds[geo_ix-1].spec_rec &&
			    NGCALLF(mpipai,MPIPAI)(&geo_ix,&LandId))
				level = 1;
			ix = NGCALLF(mpiosa,MPIOSA)(&geo_ix,&level);
			if (level < 2)
				gid = Mv41cp->entity_recs[ix-1].fixed_gid;
			else
				gid = Mv41cp->entity_recs[ix-1].dynamic_gid;
		}

                if (spec_col_ix < 0) {
                        if (Mpp->mono_fill_color) {
                                col_ix = Mpp->fill_color;
                        }
                        else {
                                int *fcp = (int*)Mpp->fill_colors->data;
                                col_ix = fcp[gid];
                        }
                }
                else {
                        int *sfcp = (int *) Mpp->spec_fill_colors->data;
                        if (Mpp->spec_fill_direct) {
                                col_ix = sfcp[spec_col_ix];
                        }
                        else if (sfcp[spec_col_ix] < Mpp->area_group_count) {
                                int *fcp = (int*)Mpp->fill_colors->data;
                                col_ix = fcp[sfcp[spec_col_ix]];
                        }
                }
                if (spec_pat_ix < 0) {
                        if (Mpp->mono_fill_pattern) {
                                pat_ix = Mpp->fill_pattern;
                        }
                        else {
                                int *fpp = (int*)Mpp->fill_patterns->data;
                                pat_ix = fpp[gid];
                        }
                }
                else {
                        int *sfpp = (int *) Mpp->spec_fill_patterns->data;
                        if (Mpp->spec_fill_direct) {
                                pat_ix = sfpp[spec_pat_ix];
                        }
                        else if (sfpp[spec_pat_ix] < Mpp->area_group_count) {
                                int *fpp = (int*)Mpp->fill_patterns->data;
                                pat_ix = fpp[sfpp[spec_pat_ix]];
                        }
                }
                if (spec_fscale_ix < 0) {
                        if (Mpp->mono_fill_scale) {
                                fscale = Mpp->fill_scale;
                        }
                        else {
                                int *fsp = (int*)Mpp->fill_scales->data;
                                fscale = fsp[gid];
                        }
                }
                else {
                        float *sfsp = (float *) Mpp->spec_fill_scales->data;
                        if (Mpp->spec_fill_direct) {
                                fscale = sfsp[spec_fscale_ix];
                        }
                        else if (sfsp[spec_fscale_ix] <Mpp->area_group_count) {
                                float *fsp = (float*)Mpp->fill_scales->data;
                                fscale = fsp[(int)sfsp[spec_fscale_ix]];
                        }
                }
                
                NhlVASetValues
                        (Mpl->base.wkptr->base.id,
                         _NhlNwkFillBackground, Mpp->fill_pattern_background,
                         _NhlNwkFillIndex, pat_ix,
                         _NhlNwkFillColor, col_ix,
                         _NhlNwkFillScaleFactorF,fscale,
                         _NhlNwkEdgesOn,0,
                         NULL);
                _NhlSetFillInfo(Mpl->base.wkptr, (NhlLayer) Mpl);
                _NhlWorkstationFill(Mpl->base.wkptr,xcs,ycs,*ncs);
                return 0;
        }
                
	return 0;
}

/*
 * Function:	mpSetUpFillDrawList
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes mpSetUpFillDrawList
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer 	mv41l,
	NhlMapPlotLayer			mpl,
	NhlString			entry_name
)
#else
(mv411,mpl,entry_name)
	NhlMapV41DataHandlerLayer 	mv41l;
        NhlMapPlotLayer 		mpl;
	NhlString			entry_name;
#endif
{
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	NhlMapPlotLayerPart	*mpp = &(mpl->mapplot);
	int	i;

        switch (mpp->fill_boundaries) {
            default:
	    case NhlNOBOUNDARIES:
	    	    Level = 0;
		    break;
            case NhlGEOPHYSICAL:
                    Level = 2;
                    break;
            case NhlNATIONAL:
                    Level = 3;
                    break;
            case NhlUSSTATES:
            case NhlGEOPHYSICALANDUSSTATES:
            case NhlALLBOUNDARIES:
                    Level = 4;
                    break;
        }
        mv41p->min_fill_level = MAX(Level,mv41p->min_fill_level);
        
        memset(DrawIds,0,Mv41cp->entity_rec_count * sizeof(mpDrawIdRec));

        for (i = 0; i < mv41p->fill_rec_count; i++) {
                int eid = mv41p->fill_recs[i].eid;
                int spec_level = c_mpiaty(eid);
                int j;
#if 0                
                if (spec_level >= mv41p->min_fill_level) {
                        DrawIds[eid-1].spec_rec = (void *)&mv41p->fill_recs[i];
                        continue;
                }
#endif                        
                for (j = 1; j <= Mv41cp->entity_rec_count; j++) {
#if 0
                        if (c_mpiosa(j,spec_level) == eid) {
                                DrawIds[j-1].spec_rec =   
                                        (void *) &mv41p->fill_recs[i];
			}
#endif
                        if (c_mpipai(j,eid)) {
                                DrawIds[j-1].spec_rec =   
                                        (void *) &mv41p->fill_recs[i];
                        }
                }
        }
        
	return ret;
}

/*
 * Function:	mpSetUpAreamap
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes mpSetUpAreamap
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer 	mv41l,
	NhlMapPlotLayer			mpl,
	NhlWorkspace			**aws,
	NhlString			entry_name
)
#else
(mv411,mpl,aws,entry_name)
	NhlMapV41DataHandlerLayer 	mv41l;
        NhlMapPlotLayer 		mpl;
	NhlWorkspace			**aws;
	NhlString			entry_name;
#endif
{
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	NhlMapPlotLayerPart	*mpp = &(mpl->mapplot);
	int			aws_id = -1,i;
	NhlBoolean		inited = False;
        float			left_npc, right_npc,left,right;
        float			left_map, right_map, lm,rm;

	mpSetUpFillDrawList(mv41l,mpl,entry_name);

	c_arseti("RC",1);
	aws_id = mv41p->aws_id;
	inited = Amap_Inited;

	if (aws_id < 0) {
		aws_id = _NhlNewWorkspace(NhlwsAREAMAP,NhlwsDISK,
					  mpWORKSPACE_SIZE_REQ);
		if (aws_id < 0) 
			return MIN(ret,(NhlErrorTypes)aws_id);
	}
	if ((*aws = _NhlUseWorkspace(aws_id)) == NULL) {
		e_text = "%s: error reserving area map workspace";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}
	if (mv41p->new_amap_req) {
                float fl,fr,fb,ft,ul,ur,ub,ut;
                int ll;
                float xp[5],yp[5];
                
		c_mpseti("VS",8);
		_NhlLLErrCheckPrnt(NhlWARNING,entry_name);
		subret = _NhlArinam(*aws,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

                subret = _NhlMplnam
                        (*aws,"Earth..1",mv41p->min_fill_level,entry_name);
                if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;
#if 0
                printf("num points in areamap %d\n",Point_Count);
#endif
                if (mpp->dump_area_map)
                        _NhlDumpAreaMap(*aws,entry_name);
        }
        
#if 0                
        {
                NhlVAGetValues(mpl->trans.trans_obj->base.id,
                               NhlNmpLeftNPCF,&left_npc,
                               NhlNmpRightNPCF,&right_npc,
                               NhlNmpLeftMapPosF,&left_map,
                               NhlNmpRightMapPosF,&right_map,
                               NULL);

                left = left_npc;
                right = MIN(right_npc,left_npc + 0.05);
                lm = left_map;
                rm = left_map + ((right-left_npc)/(right_npc-left_npc))
                        * (right_map-left_map);
                while (left < right_npc) {
                        NhlVASetValues(mpl->trans.trans_obj->base.id,
                                       NhlNmpLeftNPCF,left,
                                       NhlNmpRightNPCF,right,
                                       NhlNmpLeftMapPosF,lm,
                                       NhlNmpRightMapPosF,rm,
                                       NULL);

                        subret = _NhlMplnam(*aws,"Earth..1",Level,entry_name);
                        if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;
#if 0
                        printf("num points in areamap %d\n",Point_Count);
#endif

                        if (mpp->dump_area_map)
                                _NhlDumpAreaMap(*aws,entry_name);

                        subret = _NhlArscam
                                (*aws,(_NHLCALLF(hlumapfill,HLUMAPFILL)),
                                 entry_name);
                        if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;
                        left = right;
                        right = MIN(right_npc,left_npc + 0.05);
                        lm = rm;
                        rm = left_map + ((right-left_npc)/(right_npc-left_npc))
                                * (right_map-left_map);
                }
	}
        NhlVASetValues(mpl->trans.trans_obj->base.id,
                       NhlNmpLeftNPCF,left_npc,
                       NhlNmpRightNPCF,right_npc,
                       NhlNmpLeftMapPosF,left_map,
                       NhlNmpRightMapPosF,right_map,
                       NULL);
#endif
        
	mv41p->aws_id = aws_id;
	Amap_Inited = True;
        mv41p->new_amap_req = False;
        
	return ret;
}

/*
 * Function:	mpFill
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	


static NhlErrorTypes mpFill
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer 	mv41l,
	NhlMapPlotLayer			mpl,
	NhlString			entry_name
)
#else
(mv41l,mpl,entry_name)
        NhlMapV41DataHandlerLayer 	mv41l;
        NhlMapPlotLayer 		mpl;
	NhlString			entry_name;
#endif
{
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlMapPlotLayerPart	*mpp = &(mpl->mapplot);
        NhlWorkspace		*aws = NULL;

        mpSetUpAreamap(mv41l,mpl,&aws,entry_name);

        subret = _NhlArscam(aws,(_NHLCALLF(hlumapfill,HLUMAPFILL)),
                            entry_name);
        
        subret = _NhlIdleWorkspace(aws);
        if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	return ret;
}


/*
 * Function:  hlumaskgrid
 *
 * Description: C version of ULPR user routine called from within MAPGRM 
 *		to mask a grid based on an areamap.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/

static int (_NHLCALLF(hlumaskgrid,HLUMASKGRID))
#if	NhlNeedProto
(
	float *xcra, 
	float *ycra, 
	int *ncra, 
	int *iai, 
	int *iag, 
	int *nai
)
#else
(xcra,ycra,ncra,iai,iag,nai)
	float *xcra; 
	float *ycra; 
	int *ncra; 
	int *iai; 
	int *iag; 
	int *nai;
#endif
{
	NhlBoolean draw_line = False;
	int i,id,ix = 0;
	int type;
        int geo_ix = -1 ,vs_ix = -1;
        
	if (Mpp == NULL) return 0;

        for (i = 0; i < *nai; i++) {
                if (iag[i] == 1) geo_ix = iai[i];
                if (iag[i] == 2) vs_ix = iai[i];
        }
        if (geo_ix < 1 || vs_ix < 0)
                return 0;
#if 0
        printf("grid mask %d\n", geo_ix);
#endif        
        switch (Mpp->grid_mask_mode) {
            case NhlMASKNONE:
            default:
                    draw_line = True;
                    break;
            case NhlMASKOCEAN:
                    if (geo_ix != OceanId)
                            draw_line = True;
                    break;
            case NhlMASKNOTOCEAN:
                    if (geo_ix == OceanId)
                            draw_line = True;
                    break;
            case NhlMASKLAND:
                    if (! NGCALLF(mpipai,MPIPAI)(&geo_ix,&LandId))
                            draw_line = True;
                    break;
            case NhlMASKNOTLAND:
                    if (NGCALLF(mpipai,MPIPAI)(&geo_ix,&LandId))
                            draw_line = True;
                    break;
            case NhlMASKFILLAREA:
                    if (Mpp->fill_boundaries == NhlNOBOUNDARIES) {
                            if (! DrawIds[geo_ix-1].spec_rec)
                                    draw_line = True;
                    }
                    else if (Mpp->fill_boundaries == NhlUSSTATES) {
                            NhlBoolean found = False;
                            for (i = UsIdCount-1;i>-1;i--) {
                                    if (NGCALLF(mpipai,MPIPAI)
                                        (&geo_ix,&UsIds[i])) {
                                            found = True;
                                            break;
                                    }
                            }
                            if (!(found || DrawIds[geo_ix-1].spec_rec))
                                    draw_line = True;
                    }
                    break;
            case NhlMASKMASKAREA:
                    if (! DrawIds[geo_ix-1].spec_rec) {
                            draw_line = True;
                    }
                    else {
                            v41SpecFillRec *frec = (v41SpecFillRec *)
                                    DrawIds[geo_ix-1].spec_rec;
                            if (frec->draw_mode != mpMASK)
                                    draw_line = True;
                    }
                    break;
        }
        
	if (! draw_line)
		return 0;
		
	if (! Grid_Setup) {
		NhlVASetValues(Mpl->base.wkptr->base.id,
			       _NhlNwkLineLabel,"",
			       _NhlNwkDashPattern,Mpp->grid.dash_pat,
			       _NhlNwkLineDashSegLenF,Mpp->grid.dash_seglen,
			       _NhlNwkLineThicknessF,Mpp->grid.thickness,
			       _NhlNwkLineColor,Mpp->grid.color, 
			       NULL);

		_NhlSetLineInfo(Mpl->base.wkptr,(NhlLayer) Mpl);
		Grid_Setup = True;
	}
	_NhlWorkstationLineTo(Mpl->base.wkptr, 
			      xcra[0],ycra[0],1);
	for (i = 1; i < *ncra; i++)
		_NhlWorkstationLineTo(Mpl->base.wkptr, 
				      xcra[i],ycra[i],0);
	_NhlWorkstationLineTo(Mpl->base.wkptr,0.0,0.0,1);

	return 0;
}

/*
 * Function:	mpGrid
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes mpGrid
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer 	mv41l,
	NhlMapPlotLayer			mpl,
	NhlString			entry_name
)
#else
(mv41l,mpl,entry_name)
        NhlMapV41DataHandlerLayer 	mv41l;
        NhlMapPlotLayer 		mpl;
	NhlString			entry_name;
#endif
{
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlMapPlotLayerPart	*mpp = &(mpl->mapplot);
	NhlWorkspace		*aws;
        float flx,frx,fby,fuy,wlx,wrx,wby,wuy,lon1,lon2,lat1,lat2,spacing;
	float avlat,avlon;
	int ll,status;

	Grid_Setup = False;
	c_mpseti("C2",mpp->grid.gks_color);
	_NhlLLErrCheckPrnt(NhlWARNING,entry_name);
	c_mpseti("C4", mpp->limb.gks_color);
	_NhlLLErrCheckPrnt(NhlWARNING,entry_name);
        
	c_mpsetr("GR",mpp->grid_spacing);
	_NhlLLErrCheckPrnt(NhlWARNING,entry_name);

	if (mpp->grid_mask_mode == NhlMASKNONE) {
		c_mapgrd();
		_NhlLLErrCheckPrnt(NhlWARNING,entry_name);
		return ret;
	}
        subret = mpSetUpAreamap(mv41l,mpl,&aws,entry_name);
        if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

        subret = _NhlMapgrm(aws,(_NHLCALLF(hlumaskgrid,HLUMASKGRID)),
                            entry_name);
        if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

        subret = _NhlIdleWorkspace(aws);
        if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;
        
	return ret;
}


/*
 * Function:	mpOutline
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes mpOutline
#if	NhlNeedProto
(
        NhlMapV41DataHandlerLayer 	mv41l,
	NhlMapPlotLayer			mpl,
	NhlString			entry_name
)
#else
(mv41l,mp,entry_name)
        NhlMapV41DataHandlerLayer 	mv41l;
        NhlMapPlotLayer 		mpl;
	NhlString			entry_name;
#endif
{
        NhlMapV41DataHandlerLayerPart *mv41p = &mv41l->mapv41dh;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlMapPlotLayerPart	*mpp = &(mpl->mapplot);
	int			i;

	c_mpseti("C5",mpp->geophysical.gks_color);
	_NhlLLErrCheckPrnt(NhlWARNING,entry_name);
	c_mpseti("C6",mpp->us_state.gks_color);
	_NhlLLErrCheckPrnt(NhlWARNING,entry_name);
	c_mpseti("C7",mpp->national.gks_color);
	_NhlLLErrCheckPrnt(NhlWARNING,entry_name);

        Color = -2;
        Dash_Pattern = -2;
        Dash_SegLen = -2.0;
        Thickness = -2.0;
        
        switch (mpp->outline_boundaries) {
            default:
	    case NhlNOBOUNDARIES:
	    	    Level = 0;
	    	    break;
            case NhlGEOPHYSICAL:
                    Level = 1;
                    break;
            case NhlNATIONAL:
                    Level = 3;
                    break;
            case NhlUSSTATES:
            case NhlGEOPHYSICALANDUSSTATES:
            case NhlALLBOUNDARIES:
                    Level = 4;
                    break;
        }
        mv41p->min_outline_level = MAX(Level,mv41p->min_outline_level);

        memset(DrawIds,0,Mv41cp->entity_rec_count * sizeof(mpDrawIdRec));

        for (i = 0; i < mv41p->outline_rec_count; i++) {
                int eid = mv41p->outline_recs[i].eid;
		char eidname[128];
                int spec_level = c_mpiaty(eid);
                int j;
#if 0                
                if (spec_level >= mv41p->min_outline_level) {
                        DrawIds[eid-1].spec_rec = 
				(void *)&mv41p->outline_recs[i];
                        continue;
                }
#endif

		strcpy(eidname,c_mpname(eid));
                for (j = 1; j <= Mv41cp->entity_rec_count; j++) {
#if 0
                        if (c_mpiosa(j,spec_level) == eid) {
			        printf("%s contains %s at level %d\n",
				       eidname,c_mpname(j),spec_level);
                                DrawIds[j-1].spec_rec =   
                                        (void *) &mv41p->outline_recs[i];
			}
#endif
                        if (c_mpipai(j,eid)) {
                                DrawIds[j-1].spec_rec =   
                                        (void *) &mv41p->outline_recs[i];
                        }
                }
        }

        c_mplndr("Earth..1",mv41p->min_outline_level);
        
	return ret;
}

static NhlErrorTypes MapV41DHDrawMapList
#if	NhlNeedProto
(
	NhlLayer		instance,
        NhlMapPlotLayer 	mpl,
        mpDrawOp		draw_op,
	NhlBoolean		init_draw
        )
#else
(instance,mpl,draw_op,init_draw)
	NhlLayer		instance;
        NhlMapPlotLayer 	mpl;
        mpDrawOp		draw_op;
	NhlDrawOrder		init_draw;
#endif
{
        NhlMapV41DataHandlerLayer mv41l = 
		(NhlMapV41DataHandlerLayer) instance;
	NhlMapPlotLayerPart	  *mpp = &mpl->mapplot;
	NhlString e_text, entry_name = "MapV41DHDrawMapList";
        NhlErrorTypes ret = NhlNOERROR,subret = NhlNOERROR;
        
	Mpp = mpp;
	Mpl = mpl;

        if (! DrawIds) {
                DrawIds = NhlMalloc
		  (Mv41cp->entity_rec_count * sizeof(mpDrawIdRec));
                if (! DrawIds) {
                        NHLPERROR((NhlFATAL,ENOMEM,NULL));
                        return NhlFATAL;
                }
        }

        if (init_draw || instance != Last_Instance) {
                Amap_Inited = False;
                Part_Outside_Viewspace = False;
                if (Mpl->view.x < 0.0 || Mpl->view.y > 1.0 ||
                    Mpl->view.x + Mpl->view.width > 1.0 ||
                    Mpl->view.y - Mpl->view.height < 0.0)
                        Part_Outside_Viewspace = True;
        }
        if (instance != Last_Instance)
                mv41l->mapv41dh.new_amap_req = True;
        
        Last_Instance = instance;
        Point_Count = 0;

        switch (draw_op) {
            case mpDRAWFILL:
                    return mpFill(mv41l,mpl,entry_name);
            case mpDRAWOUTLINE:
                    return mpOutline(mv41l,mpl,entry_name);
            case mpDRAWGRID:
                    return mpGrid(mv41l,mpl,entry_name);
        }

        return ret;
}

/*
 * Function:  ModifyFill
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static void   ModifyFill
#if	NhlNeedProto
(
        int ilty,
        int ioal,
        int ioar,
        int *npts,
	float *pnts
)

#else
(ilty,ioal,ioar,npts,pnts)
        int ilty;
        int ioal;
        int ioar;
        int *npts;
	float *pnts;
#endif
{
	NhlMapBoundarySets boundaries = Mpp->fill_boundaries;
	NhlBoolean found = False;
	int i;

        if (DrawIds[ioal-1].spec_rec || DrawIds[ioar-1].spec_rec) {
	        Point_Count += *npts;
		return;
        }

	switch (boundaries) {
	    default:
	    case NhlNOBOUNDARIES:
	    	*npts = 0;
		break;
	    case NhlGEOPHYSICAL:
	    case NhlNATIONAL:
	    case NhlALLBOUNDARIES:
	        break;
	    case NhlUSSTATES:
	        for (i = UsIdCount-1;i>-1;i--) {
		  	if (NGCALLF(mpipai,MPIPAI)(&ioal,&UsIds[i]) ||
			    NGCALLF(mpipai,MPIPAI)(&ioar,&UsIds[i])) {
			  	found = True;
				break;
			}
		}
		if (! found) {
		  	*npts = 0;
		}
		break;
	    case NhlGEOPHYSICALANDUSSTATES:
	        if (ilty > 1) {
			for (i = UsIdCount-1;i>-1;i--) {
				if (NGCALLF(mpipai,MPIPAI)(&ioal,&UsIds[i]) ||
				    NGCALLF(mpipai,MPIPAI)(&ioar,&UsIds[i])) {
					found = True;
					break;
				}
			}
			if (! found) {
				*npts = 0;
			}
		}
		break;
        }

	if (*npts) {
	        Point_Count += *npts;
	}

	return;
}


static void SetLineAttrs
(
        int type,
	int *npts
        )
{
        int color, dash_pattern;
        float thickness,dash_seglen;
        char *entry_name = "MapV41DHDrawMapList";

        switch (type) {
            case 1:
            case 2:
                    color = Mpp->geophysical.gks_color;
                    dash_pattern = Mpp->geophysical.dash_pat;
                    dash_seglen = Mpp->geophysical.dash_seglen;
                    thickness = Mpp->geophysical.thickness;
                    break;
            case 3:
                    color = Mpp->national.gks_color;
                    dash_pattern = Mpp->national.dash_pat;
                    dash_seglen = Mpp->national.dash_seglen;
                    thickness = Mpp->national.thickness;
                    break;
            case 4:
                    color = Mpp->us_state.gks_color;
                    dash_pattern = Mpp->us_state.dash_pat;
                    dash_seglen = Mpp->us_state.dash_seglen;
                    thickness = Mpp->us_state.thickness;
                    break;
        }
        if (color != Color) {
                gset_line_colr_ind(color);
                Color = color;
        }
        if (dash_pattern != Dash_Pattern ||
            dash_seglen != Dash_SegLen) {
                int	dpat;
                NhlString *sp;
                float	p0,p1,jcrt;
                int	slen;
                char	buffer[128];
                
                dpat = dash_pattern % Mpp->dash_table->num_elements;
                sp = (NhlString *) Mpp->dash_table->data;
                slen = strlen(sp[dpat]);
                p0 =  (float) c_kfpy(0.0);
                _NhlLLErrCheckPrnt(NhlWARNING,entry_name);
                p1 = dash_seglen;
                p1 = (float) c_kfpy(p1);
                _NhlLLErrCheckPrnt(NhlWARNING,entry_name);
                jcrt = (int) ((p1 - p0) / slen + 0.5);
                jcrt = jcrt > 1 ? jcrt : 1;
                strcpy(buffer,sp[dpat]);
	
                c_dashdc(buffer,jcrt,4);
                _NhlLLErrCheckPrnt(NhlWARNING,entry_name);
                Dash_Pattern = dash_pattern;
                Dash_SegLen = Dash_SegLen;
        }
        if (thickness != Thickness) {
              	gset_linewidth(thickness);
                _NhlLLErrCheckPrnt(NhlWARNING,entry_name);
                Thickness = thickness;
        }
        return;
}

/*
 * Function:  ModifyLines
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static void   ModifyLines
#if	NhlNeedProto
(
        int ilty,
        int ioal,
        int ioar,
        int *npts,
	float *pnts
)

#else
(ilty,ioal,ioar,npts,pnts)
        int ilty;
        int ioal;
        int ioar;
        int *npts;
	float *pnts;
#endif
{
	NhlMapBoundarySets boundaries = Mpp->outline_boundaries;
	NhlBoolean found = False;
	int i;

        if (DrawIds[ioal-1].spec_rec && DrawIds[ioar-1].spec_rec) {
	  	v41SpecLineRec *lrec = 
		  	(v41SpecLineRec *)DrawIds[ioal-1].spec_rec;
	  	v41SpecLineRec *rrec = 
		  	(v41SpecLineRec *)DrawIds[ioar-1].spec_rec;
		if (lrec->eid == rrec->eid) {
			*npts = 0;
			return;
		}
	        Point_Count += *npts;
		SetLineAttrs(ilty,npts);
		return;
        }
        else if (DrawIds[ioal-1].spec_rec || DrawIds[ioar-1].spec_rec) {
	        Point_Count += *npts;
		SetLineAttrs(ilty,npts);
		return;
	}
	else if (ilty > Level) {
		*npts = 0;
		return;
	}

	switch (boundaries) {
	    default:
	    case NhlNOBOUNDARIES:
	    	*npts = 0;
		break;
	    case NhlGEOPHYSICAL:
	    case NhlNATIONAL:
	    case NhlALLBOUNDARIES:
	        break;
	    case NhlUSSTATES:
	        for (i = UsIdCount-1;i>-1;i--) {
		  	if (NGCALLF(mpipai,MPIPAI)(&ioal,&UsIds[i]) ||
			    NGCALLF(mpipai,MPIPAI)(&ioar,&UsIds[i])) {
			  	found = True;
				break;
			}
		}
		if (! found) {
		  	*npts = 0;
		}
		break;
	    case NhlGEOPHYSICALANDUSSTATES:
	        if (ilty > 1) {
			for (i = UsIdCount-1;i>-1;i--) {
				if (NGCALLF(mpipai,MPIPAI)(&ioal,&UsIds[i]) ||
				    NGCALLF(mpipai,MPIPAI)(&ioar,&UsIds[i])) {
					found = True;
					break;
				}
			}
			if (! found) {
				*npts = 0;
			}
		}
		break;
        }

	if (*npts) {
	        Point_Count += *npts;
		SetLineAttrs(ilty,npts);
	}

	return;
}

/*
 * Function:  hlumpchln
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
void   (_NHLCALLF(hlumpchln,HLUMPCHLN))
#if	NhlNeedProto
(
        int *iflg,
        int *ilty,
        int *ioal,
        int *ioar,
        int *npts,
	float *pnts
)

#else
(iflg,ilty,ioal,ioar,npts,pnts)
        int *iflg;
        int *ilty;
        int *ioal;
        int *ioar;
        int *npts;
	float *pnts;
#endif
        
{

	if (Mpp == NULL) {
		_NHLCALLF(mpchln,MPCHLN)(iflg,ilty,ioal,ioar,npts,pnts);
		return;
	}

        if (*ilty == 0) {
                *npts = 0;
                return;
        }

	switch (*iflg) {
	case 1:
	case 2:
	  ModifyFill(*ilty,*ioal,*ioar,npts,pnts);
	  break;
	case 3:
	default:
	  ModifyLines(*ilty,*ioal,*ioar,npts,pnts);
	  break;
	}

	return;
}

/*
 * Function:  load_hlumap_routines
 *
 * Description: Forces the hlumap... routines to load from the HLU library
 *
 * In Args:   NhlBoolean flag - should always be False - dont actually
 *			        want to call the routines.
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static void   load_hlumap_routines
#if	NhlNeedProto
(
	NhlBoolean	flag
)
#else
(flag)
	NhlBoolean	flag;
#endif
{
	int idum;
	float fdum;


	if (flag) {
		_NHLCALLF(hlumapeod,HLUMAPEOD)
			(&idum,&idum,&idum,&idum,&idum,&fdum);
	}
	return;
}


/*
 *      $Id: ctrans.h,v 1.5 1993-01-12 20:10:52 clyne Exp $
 */
/*
 *	File:		ctrans.h
 *
 *	Author:		John Clyne
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Jul 10 11:22:23 MDT 1992
 *
 *	Description:	header file for ctrans.c
 */

#include <ncarg/c.h>
#include <ncarg/cgm_tools.h>

#ifndef	_ctrans_
#define	_ctrans_

typedef	enum	{
	FATAL = -2,
	WARN = -1,
	EOM = 0,
	OK = 1
	} CtransRC;


#define	EOM	0	/* End of Metafile	*/

/* This macro protects C function names from C++ name-mangling. */
NCARG_PROTO_BEGIN

extern	CtransRC	init_ctrans(
#ifdef	NeedFuncProto
	int	*argc,
	char	**argv,
	const char	*gcap,
	const char	*fcap,
	boolean	batch
#endif
);




extern	CtransRC	init_metafile(
#ifdef	NeedFuncProto
	int	record,
	Cgm_fd	cgm_fd
#endif
);

extern	void	close_metafile(
#ifdef	NeedFuncProto
#endif
);

extern	CtransRC	ctrans(
#ifdef	NeedFuncProto
	int	record
#endif
);

extern	CtransRC	ctrans_merge(
#ifdef	NeedFuncProto
	int	record1,
	int	record2
#endif
);

extern	CtransRC	SetDevice(
#ifdef	NeedFuncProto
	const char	*gcap
#endif
);

extern	CtransRC	SetFont(
#ifdef	NeedFuncProto
	const char	*fcap
#endif
);

extern	void	SetDefaultPalette(
#ifdef	NeedFuncProto
	const char	*pal_fname
#endif
);

extern	void	GraphicsMode(
#ifdef	NeedFuncProto
	boolean	on
#endif
);

extern	void	close_ctrans(
#ifdef	NeedFuncProto
#endif
);

extern	char	*getGcapname(
#ifdef	NeedFuncProto
	const char	*gcap
#endif
);

extern	char	*getFcapname(
#ifdef	NeedFuncProto
	const char*	fcap
#endif
);

extern	boolean	IsOutputToTty(
#ifdef	NeedFuncProto
#endif
);

extern	char	*ReadCtransMsg(
#ifdef	NeedFuncProto
#endif
);

extern	void	CtransClear(
#ifdef	NeedFuncProto
#endif
);

/*
**
**	yet another ctrans programming interface. 
**
*/



extern	CtransRC	CtransOpenBatch(
#ifdef	NeedFuncProto
	const char	*device_name,
	const char	*font_name,
	const char	*metafile,
	int	*dev_argc,
	char	**dev_argv
#endif
);

extern	CtransRC	CtransSetMetafile(
#ifdef	NeedFuncProto
	const char	*metafile
#endif
);

extern	CtransRC	CtransPlotFrame(
#ifdef	NeedFuncProto
#endif
);

extern	CtransRC	CtransClearDisplay(
#ifdef	NeedFuncProto
#endif
);

void	CtransCloseBatch(
#ifdef	NeedFuncProto
#endif
);

extern	CtransRC	CtransLSeekBatch(
#ifdef	NeedFuncProto
	unsigned	offset,
	unsigned	whence
#endif
);

void	CtransGraphicsMode(
#ifdef	NeedFuncProto
	boolean	on
#endif
);

extern	int	CtransGetErrorNumber(
#ifdef	NeedFuncProto
#endif
);


extern	char	*CtransGetErrorMessage(
#ifdef	NeedFuncProto
#endif
);

NCARG_PROTO_END

#endif

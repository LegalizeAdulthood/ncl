.\"
.\"	$Id: gqtxfp.m,v 1.10 2000-08-22 04:16:01 haley Exp $
.\"
.TH GQTXFP 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
GQTXFP (Inquire text font and precision) - retrieves the current setting
of the text font and precision values.
.SH SYNOPSIS
CALL GQTXFP (ERRIND, FONT, PREC)
.SH C-BINDING SYNOPSIS
#include <ncarg/gks.h>
.sp
void ginq_text_font_prec(Gint *err_ind, Gtext_font_prec *text_font_prec);
.SH DESCRIPTION
.IP ERRIND 12
(Integer, Output) - If the inquired values cannot be returned correctly,
a non-zero error indicator is returned in ERRIND, otherwise a zero is returned.
Consult "User's Guide for NCAR GKS-0A Graphics" for a description of the
meaning of the error indicators.
.IP FONT 12
(Integer, Output) - 
Gives the font number as set by default or by a call to
GSTXFP.  FONT number one is a simple default ASCII font. Values for
FONT between -2 and -20 (inclusive) are Hershey fonts.
.IP PREC 12
(Integer, Output) - Gives the text precision as set by default or by a call to
GSTXFP.
.RS
.IP 0
String precision (good)
.IP 1
Character precision (better)
.IP 2
Stroke precision (best)
.RE
.SH ACCESS
To use GKS routines, load the NCAR GKS-0A library ncarg_gks.
.SH SEE ALSO
Online: 
gtx, gstxp, gstxal, gstxfp, gschh, gschsp, gschup, 
gschxp, gscr, gstxci, gqtxp, gqtxal, gqtxfp, gqchh, 
gqchsp, gqchup, gqchxp, plotchar, ginq_text_font_prec
.sp
Hardcopy: 
User's Guide for NCAR GKS-0A Graphics;
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright (C) 1987-2000
.br
University Corporation for Atmospheric Research
.br

This documentation is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.

.TH IDPLTR 3NCARG "November 1995" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
Plots the triangulation generated by a prior call to either IDBVIP or IDSFFT.
.SH SYNOPSIS
CALL IDPLTR (NDAT,XDAT,YDAT,IWRK)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_idbvip (int ndat, float *xdat, float *ydat, float *iwrk)
.SH DESCRIPTION
.IP NDAT 12
Number of data points (argument NDP from a prior call to
IDBVIP or IDSFFT).
.IP XD 12
Array of dimension NDP containing the X coordinates of
the data points (from prior call to IDBVIP or IDSFFT).
.IP YD 12
Array of dimension NDP containing the Y coordinates of
the data points (from prior call to IDBVIP or IDSFFT).
.IP ZD 12
Array of dimension NDP containing the Z coordinates of
the data points (from prior call to IDBVIP or IDSFFT).
.IP IWRK 12
Integer work array of dimension at least 31*NDP + NIP (argument IWK
from prior call to IDBVIP or IDSFFT).
.SH ""
Note that the caller is responsible for doing the SET call required
to map the triangles to the correct position on the plotter frame.
.sp
Note also that some triangle edges are drawn twice.  No attempt is
made to prevent this.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH ACCESS 
To use IDPLTR or c_idpltr, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.  
.SH MESSAGES
See the bivar man page for a description of all Bivar error
messages and/or informational messages.
.SH SEE ALSO
Online:
bivar, bivar_params, idbvip, idsfft, idgeti, idgetr, idseti, idsetr,
ncarg_cbind
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

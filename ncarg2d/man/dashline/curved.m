.TH CURVED 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
CURVED - Draws a curve through points.
.SH SYNOPSIS
CALL CURVED (X, Y, N)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_curved (float *x, float *y, int n)
.SH DESCRIPTION 
.IP X 12
(an input array of type REAL) defining the X user coordinates of
the curve.  Array X is of length N.
.IP Y 12
(an input array of type REAL) defining the Y user coordinates of
the curve.  Array Y is of length N.
.IP N 12
(an input parameter of type INTEGER) the value of which is the number
of points in the curve.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument description.
.SH EXAMPLES
Use the ncargex command to see the following relevant examples: 
tdashc, tdashl, tdashp, tdashs, carline,
cmpfil, cmpgrp, cmpitm, cmplab, cmpmsk, cmptit,
cpex01, cpex02, cpex03, cpex04, cpex06, vvex01,
fdlcurvd, fdldashd, fdlsmth, fpcloqu
.SH ACCESS
To use CURVED, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_loc, preferably in that order.  To use c_curved, load 
the NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, ncarg_gks,
and ncarg_loc, preferably in that order.
.sp
See the man page for dashline to see how to access the quick,
normal, smooth, and super line draw options.
.SH SEE ALSO
Online:
dashline, dashline_params,
dashdb, dashdc, frstd, lastd, lined, reset, vectd, ncarg_cbind
.sp
Hardcopy:  
NCAR Graphics Contouring and Mapping Tutorial;
NCAR Graphics Fundamentals, UNIX Version;
User's Guide for NCAR GKS-0A Graphics
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved

C
C $Id: agkurv.f,v 1.6 2001-06-08 21:21:59 kennison Exp $
C
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C This file is free software; you can redistribute it and/or modify
C it under the terms of the GNU General Public License as published
C by the Free Software Foundation; either version 2 of the License, or
C (at your option) any later version.
C
C This software is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C General Public License for more details.
C
C You should have received a copy of the GNU General Public License
C along with this software; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
C USA.
C
      SUBROUTINE AGKURV (XVEC,IIEX,YVEC,IIEY,NEXY,SVAL)
C
      DIMENSION XVEC(1),YVEC(1)
C
C AGKURV plots the curve defined by the points ((X(I),Y(I)),I=1,NEXY),
C where
C
C    X(I)=XVEC(1+(I-1)*IIEX) (unless IIEX=0, in which case X(I)=I), and
C    Y(I)=YVEC(1+(I-1)*IIEY) (unless IIEY=0, in which case Y(I)=I).
C
C If, for some I, X(I)=SVAL or Y(I)=SVAL, curve line segments having
C (X(I),Y(I)) as an endpoint are omitted.
C
C No windowing is performed.
C
C Check first whether the number of curve points is properly specified.
C
      IF (NEXY.LE.0) GO TO 901
C
C Initialization.  Pretend that the last point was point number zero.
C Set the indices for the x and y vectors accordingly.  Clear the line-
C drawn-to-last-point flag.
C
      INDP=0
      INDX=1-IIEX
      INDY=1-IIEY
      LDLP=0
C
C Initialization.  Retrieve the current curve window, user window, and
C x/y linear/logarithmic flags.
C
      CALL GETSET (XLCW,XRCW,YBCW,YTCW,XLUW,XRUW,YBUW,YTUW,LTYP)
C
C Initialization.  Set linear/log flag and linear-window limits for
C x-axis values.
C
      IF (LTYP.EQ.1.OR.LTYP.EQ.2) THEN
        LLUX=0
        XLLW=XLUW
        XRLW=XRUW
      ELSE
        LLUX=1
        XLLW=ALOG10(XLUW)
        XRLW=ALOG10(XRUW)
      END IF
C
C Initialization.  Set linear/log flag and linear-window limits for
C y-axis values.
C
      IF (LTYP.EQ.1.OR.LTYP.EQ.3) THEN
        LLUY=0
        YBLW=YBUW
        YTLW=YTUW
      ELSE
        LLUY=1
        YBLW=ALOG10(YBUW)
        YTLW=ALOG10(YTUW)
      END IF
C
C Initialization.  Call SET, if necessary, to define a linear mapping.
C
      IF (LTYP.NE.1) THEN
        CALL SFLUSH
        CALL SET (XLCW,XRCW,YBCW,YTCW,XLLW,XRLW,YBLW,YTLW,1)
      END IF
C
C Beginning of loop through points.  Update indices and determine the
C user-space coordinates of the next point.
C
  101 IF (INDP.EQ.NEXY) GO TO 102
      INDP=INDP+1
C
      INDX=INDX+IIEX
      XNXT=XVEC(INDX)
      IF (IIEX.EQ.0) XNXT=FLOAT(INDP)
      IF (LLUX.NE.0.AND.XNXT.LE.0.) XNXT=SVAL
C
      INDY=INDY+IIEY
      YNXT=YVEC(INDY)
      IF (IIEY.EQ.0) YNXT=FLOAT(INDP)
      IF (LLUY.NE.0.AND.YNXT.LE.0.) YNXT=SVAL
C
C Check whether (XNXT,YNXT) is a special-value point.  Handle that case.
C
      IF (XNXT.EQ.SVAL.OR.YNXT.EQ.SVAL) THEN
        IF (LDLP.EQ.0) GO TO 101
        IF (LDLP.EQ.1) CALL VECTD (XLST,YLST)
        CALL LASTD
        LDLP=0
        GO TO 101
      END IF
C
C If user space is not linear/linear, modify XNXT and YNXT accordingly.
C
      IF (LLUX.NE.0) XNXT=ALOG10(XNXT)
      IF (LLUY.NE.0) YNXT=ALOG10(YNXT)
C
C Start or continue line.
C
      IF (LDLP.EQ.0) THEN
        CALL FRSTD (XNXT,YNXT)
        XLST=XNXT
        YLST=YNXT
      ELSE
        CALL VECTD (XNXT,YNXT)
      END IF
C
      LDLP=LDLP+1
      GO TO 101
C
C Last point was final point.  Finish up.
C
  102 IF (LDLP.NE.0) THEN
        IF (LDLP.EQ.1) CALL VECTD (XLST,YLST)
        CALL LASTD
      END IF
C
C Restore logarithmic mapping, if appropriate.
C
      IF (LTYP.NE.1) THEN
        CALL SFLUSH
        CALL SET (XLCW,XRCW,YBCW,YTCW,XLUW,XRUW,YBUW,YTUW,LTYP)
      END IF
C
C Make sure buffers are flushed.
C
      CALL SFLUSH
C
C Return to caller.
C
      RETURN
C
C Error exit.
C
  901 CALL SETER ('AGKURV - NUMBER OF POINTS IS LESS THAN OR EQUAL TO ZE
     +RO',3,2)
C
      END

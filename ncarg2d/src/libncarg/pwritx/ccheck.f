C
C	$Id: ccheck.f,v 1.4 2008-07-27 00:17:20 haley Exp $
C                                                                      
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C The use of this Software is governed by a License Agreement.
C
      SUBROUTINE CCHECK (IERR)
C
C SHORT TEST ROUTINE FOR THE CORRECT ACCESSING OF THE COMPLEX CHARACTER
C SET.
C
C ON ENTRY
C   IERR MAY CONTAIN ANY VALUE.
C ON EXIT
C   IERR = 0 IF 2 SHORT TESTS OF THE DIGITIZATION RETRIEVAL PROCESS
C            WERE SUCCESSFUL
C        = 2 IF THE TEST WAS SUCCESSFUL FOR 6-BIT DIGITIZATION UNITS
C            BUT NOT SUCCESSFUL FOR 12-BIT DIGITIZATION UNITS
C        = 1 IF THE TEST WAS SUCCESSFUL FOR 12-BIT DIGITIZATION UNITS
C            BUT NOT SUCCESSFUL FOR 6-BIT DIGITIZATION UNITS.
C        = 3 IF NEITHER OF THE 2 TESTS WAS SUCCESSFUL.
C CALLS
C   XTCH
C
C
C PWRC1 IS FOR COMMUNICATION WITH SUBROUTINE XTCH.
      COMMON /PWRC1/ LC(150)
C
C THE ARRAY CONTAINING THE CORRECT DIGITIZATION OF CHARACTER +.
      DIMENSION LCPCOR(14)
C THE ARRAY CONTAINING THE CORRECT DIGITIZATION OF THE CHARACTER DEFINED
C BY THE OCTAL NUMBER 751.
      DIMENSION LCOCOR(30)
C
C TEST 1
C   RETRIEVE DIGITIZATION OF CHARACTER + WITH DEFAULT FUNCTION CODES
C   AND COMPARE IT TO THE CORRECT DIGITIZATION.
C   (TEST 6-BIT DIGITIZATION UNITS)
C TEST 2
C   RETRIEVE DIGITIZATION OF CHARACTER DEFINED BY THE OCTAL NUMBER 751
C   AND COMPARE IT WITH THE CORRECT DIGITIZATION.
C   (TEST 12-BIT DIGITIZATION UNITS)
C
C        ** DATA FOR TEST 1 **
C
C THE NUMBER OF THE UNIT IN ARRAY IND POINTING TO THE DIGITIZATION
C OF CHARACTER +.
      DATA IPLUSP /37/
C
C THE CORRECT NUMBER OF DIGITIZATION UNITS FOR CHARACTER +.
      DATA IPLUSN /14/
C
C THE CORRECT DIGITIZATION OF CHARACTER +.
      DATA LCPCOR(1) /-13/
      DATA LCPCOR(2) /13/
      DATA LCPCOR(3) /0/
      DATA LCPCOR(4) /9/
      DATA LCPCOR(5) /0/
      DATA LCPCOR(6) /-9/
      DATA LCPCOR(7) /-2048/
      DATA LCPCOR(8) /0/
      DATA LCPCOR(9) /-9/
      DATA LCPCOR(10) /0/
      DATA LCPCOR(11) /9/
      DATA LCPCOR(12) /0/
      DATA LCPCOR(13) /-2048/
      DATA LCPCOR(14) /-2048/
C
C        ** DATA FOR TEST 2 **
C
C THE NUMBER OF THE UNIT IN ARRAY IND POINTING TO THE DIGITIZATION OF
C THE CHARACTER DEFINED BY THE OCTAL NUMBER 751.
      DATA IOCTP /489/
C
C THE CORRECT NUMBER OF DIGITIZATION UNITS OF THE CHARACTER DEFINED BY
C THE OCTAL NUMBER 751.
      DATA IOCTN /30/
C
C THE CORRECT DIGITIZATION OF THE CHARACTER DEFINED BY THE OCTAL
C NUMBER 751.
      DATA LCOCOR(1) /-9/
      DATA LCOCOR(2) /9/
      DATA LCOCOR(3) /-5/
      DATA LCOCOR(4) /39/
      DATA LCOCOR(5) /-5/
      DATA LCOCOR(6) /0/
      DATA LCOCOR(7) /-5/
      DATA LCOCOR(8) /-39/
      DATA LCOCOR(9) /-2048/
      DATA LCOCOR(10) /0/
      DATA LCOCOR(11) /-4/
      DATA LCOCOR(12) /39/
      DATA LCOCOR(13) /-4/
      DATA LCOCOR(14) /0/
      DATA LCOCOR(15) /-4/
      DATA LCOCOR(16) /-39/
      DATA LCOCOR(17) /-2048/
      DATA LCOCOR(18) /0/
      DATA LCOCOR(19) /-5/
      DATA LCOCOR(20) /39/
      DATA LCOCOR(21) /6/
      DATA LCOCOR(22) /39/
      DATA LCOCOR(23) /-2048/
      DATA LCOCOR(24) /0/
      DATA LCOCOR(25) /-5/
      DATA LCOCOR(26) /-39/
      DATA LCOCOR(27) /6/
      DATA LCOCOR(28) /-39/
      DATA LCOCOR(29) /-2048/
      DATA LCOCOR(30) /-2048/
C
C
C THE PASS THROUGH PWRX TO BE SIMULATED IN BOTH TESTS.
      DATA IPASS /2/
C
C
      IER1 = 0
      IER2 = 0
C
C        ***** TEST 1 *****
C
C RETRIEVE DIGITIZATION OF CHARACTER +.
      CALL XTCH (IPLUSP,IPASS,NACT1)
C
C COMPARE THE NUMBER OF DIGITIZATION UNITS RETRIEVED TO THE CORRECT
C NUMBER.
      IF (NACT1 .EQ. IPLUSN) GOTO 1
      IER1 = 1
      GOTO 10
    1 CONTINUE
C
C COMPARE EACH OF THE RETRIEVED DIGITIZATION UNITS TO THE CORRECT UNIT.
      DO 2 I=1,NACT1
      IF (LC(I) .EQ. LCPCOR(I)) GOTO 2
      IER1 = 1
      GOTO 10
    2 CONTINUE
C
C        ***** TEST 2 *****
C
   10 CONTINUE
C RETRIEVE DIGITIZATION OF CHARACTER DEFINED BY OCTAL NUMBER 751.
      CALL XTCH (IOCTP,IPASS,NACT2)
C
C COMPARE THE NUMBER OF DIGITIZATION UNITS RETRIEVED TO THE CORRECT
C NUMBER.
      IF (NACT2 .EQ. IOCTN) GOTO 11
      IER2 = 2
      GOTO 30
   11 CONTINUE
C
C COMPARE EACH OF THE RETRIEVED DIGITIZATION UNITS TO THE CORRECT UNIT.
      DO 20 I = 1,NACT2
      IF (LC(I) .EQ. LCOCOR(I)) GOTO 20
      IER2 = 2
      GOTO 30
   20 CONTINUE
C
C
   30 CONTINUE
      IERR = IER1 + IER2
C
C
      RETURN
      END

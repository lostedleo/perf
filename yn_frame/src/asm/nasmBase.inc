; =============================================================================
; nasmBase.inc
; -----------------------------------------------------------------------------
; 
; 
; Revision History:
; -----------------------------------------------------------------------------
; 
; 
; =============================================================================


%ifndef __nasmBase_inc_
%define __nasmBase_inc_


; =============================================================================
; macros

; -----------------------------------------------------------------------------
; set prefix '_' for global functions (for MSC)
%macro cglobal 1
  %ifdef NOPREFIX
    global  %1
  %else
    global  _%1
    %define %1 _%1
  %endif
%endmacro

%macro cextern 1
	%ifdef NOPREFIX
		extern %1
	%else
		extern _%1
		%define %1 _%1
	%endif
%endmacro

%endif  ;__nasmBase_inc_

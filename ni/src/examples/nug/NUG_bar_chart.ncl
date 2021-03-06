;-----------------------------------------------------------------
;  NCL User Guide Example:   NUG_bar_chart.ncl
;
;  KMF 30.10.14
;-----------------------------------------------------------------
; These load commands are not required in NCL versions 6.2.0 and later.
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_code.ncl"
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_csm.ncl"

begin

   low  =  0.0
   high =  1.0

   n = 12

   x = fspan(1.0, 12.0, n)
   y = random_uniform(low, high, n)

   wks = gsn_open_wks("png","NUG_bar_chart")

   res                       =  True
   res@gsnXYBarChart         =  True
   res@gsnXYBarChartBarWidth =  0.3
   res@gsnXYBarChartColors   = "blue"

   res@trXMinF               =  0.0              ;-- x-axis min value
   res@trXMaxF               = 13.0              ;-- x-axis max value
   res@trYMinF               =  0.0              ;-- y-axis min value
   res@trYMaxF               =  1.0              ;-- y-axis max value

   res@tmXBMode              = "Explicit"        ;-- explicit labels
   res@tmXBValues            =  ispan(1,12,1)
   res@tmXBLabels            = (/"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep", \
                                 "Oct","Nov","Dec"/)
   res@tmXBLabelFontHeightF  =  0.015

   res@tiMainString          = "NCL Doc Example: bar chart"

   plot = gsn_csm_xy(wks, x, y, res)

end

# Begin_DVE_Session_Save_Info
# DVE view(Wave.1 ) session
# Saved on Thu Nov 21 16:35:46 2013
# Toplevel windows open: 2
# 	TopLevel.1
# 	TopLevel.2
#   Wave.1: 18 signals
# End_DVE_Session_Save_Info

# DVE version: F-2011.12_Full64
# DVE build date: Nov 22 2011 20:55:00


#<Session mode="View" path="/net/nobackup/ice_laptop/seong/pof/session.spi_aes_uncompressed_skip_sanity.vpd.tcl" type="Debug">

#<Database>

#</Database>

# DVE View/pane content session: 

# Begin_DVE_Session_Save_Info (Wave.1)
# DVE wave signals session
# Saved on Thu Nov 21 16:35:46 2013
# 18 signals
# End_DVE_Session_Save_Info

# DVE version: F-2011.12_Full64
# DVE build date: Nov 22 2011 20:55:00


#Add ncecessay scopes

gui_set_time_units 10ps
set Group1 Group1
if {[gui_sg_is_group -name Group1]} {
    set Group1 [gui_sg_generate_new_name]
}

gui_sg_addsignal -group "$Group1" { {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.data_out} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.keyin} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.ciphertext} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.g_cs_usermode} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.enc1.load} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.enc1.textff} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.enc1.plaintext} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.aes.ciphertext} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.cs.crc_cs.ei_data_d1} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.cs.crc_cs.ei_data_d2} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.ci.cb_din} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.ci.cs_din} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.cs.crc_cs.crcerror} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.cs.csm_top.csm_or_iocsr.cs} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.ismclk_domain.spi.spi_data_regs.cntr_done} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.ismclk_domain.spi.spi_data_regs.counter} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.ismclk_domain.spi.spi_data_regs.poflength} {V1:z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.gclk_domain.cs.csm_top.csm_core.cs} }
if {![info exists useOldWindow]} { 
	set useOldWindow true
}
if {$useOldWindow && [string first "Wave" [gui_get_current_window -view]]==0} { 
	set Wave.1 [gui_get_current_window -view] 
} else {
	gui_open_window Wave
set Wave.1 [ gui_get_current_window -view ]
}
set groupExD [gui_get_pref_value -category Wave -key exclusiveSG]
gui_set_pref_value -category Wave -key exclusiveSG -value {false}
set origWaveHeight [gui_get_pref_value -category Wave -key waveRowHeight]
gui_list_set_height -id Wave -height 25
set origGroupCreationState [gui_list_create_group_when_add -wave]
gui_list_create_group_when_add -wave -disable
gui_marker_create -id ${Wave.1} C2 84815880
gui_marker_set_ref -id ${Wave.1}  C1
gui_wv_zoom_timerange -id ${Wave.1} 0 485028460
gui_list_add_group -id ${Wave.1} -after {New Group} [list $Group1]
gui_seek_criteria -id ${Wave.1} {Any Edge}


gui_set_pref_value -category Wave -key exclusiveSG -value $groupExD
gui_list_set_height -id Wave -height $origWaveHeight
if {$origGroupCreationState} {
	gui_list_create_group_when_add -wave -enable
}
if { $groupExD } {
 gui_msg_report -code DVWW028
}
gui_list_set_filter -id ${Wave.1} -list { {Buffer 1} {Input 1} {Others 1} {Linkage 1} {Output 1} {Parameter 1} {All 1} {Aggregate 1} {Event 1} {Assertion 1} {Constant 1} {Interface 1} {Signal 1} {$unit 1} {Inout 1} {Variable 1} }
gui_list_set_filter -id ${Wave.1} -text {*}
gui_list_set_insertion_bar  -id ${Wave.1} -group $Group1  -item {z1409f_testbench.xf.xq1.xc9300_r10_c32.xc8100_r10.xcb.xfcb.c8000.ismclk_domain.spi.spi_data_regs.poflength[31:0]} -position below

gui_marker_move -id ${Wave.1} {C1} 84710754
gui_view_scroll -id ${Wave.1} -vertical -set 0
gui_show_grid -id ${Wave.1} -enable false
#</Session>


LOAD DATA
INFILE './load_data/mdt_rt_map_r.dat'
BADFILE './load_data/mdt_rt_map_r.bad'
DISCARDFILE './load_data/mdt_rt_map_r.dsc'
CONTINUEIF NEXT(1)='+'
INTO TABLE mdt_rt_map_r  APPEND
FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
(MDT_RT_MAP_R_ID,
MDT_RT_ID, 	  
MDT_RT_MAP_T_ID, 
BINS,		  
R_1,		  
R_2, 
R_3,
R_4,
R_5,
R_6,
R_7,
R_8,
R_9,
R_10,
R_11, 
R_12, 
R_13, 
R_14, 
R_15, 
R_16, 
R_17, 
R_18, 
R_19, 
R_20, 
R_21, 
R_22, 
R_23, 
R_24, 
R_25, 
R_26, 
R_27, 
R_28, 
R_29, 
R_30, 
R_31, 
R_32, 
R_33, 
R_34, 
R_35, 
R_36, 
R_37, 
R_38, 
R_39, 
R_40, 
R_41, 
R_42, 
R_43, 
R_44, 
R_45, 
R_46, 
R_47, 
R_48, 
R_49, 
R_50, 
R_51, 
R_52, 
R_53, 
R_54, 
R_55, 
R_56, 
R_57, 
R_58, 
R_59, 
R_60, 
R_61, 
R_62, 
R_63, 
R_64, 
R_65, 
R_66, 
R_67, 
R_68, 
R_69, 
R_70, 
R_71, 
R_72, 
R_73, 
R_74, 
R_75, 
R_76, 
R_77, 
R_78, 
R_79, 
R_80, 
R_81, 
R_82, 
R_83, 
R_84, 
R_85, 
R_86, 
R_87, 
R_88, 
R_89, 
R_90, 
R_91, 
R_92, 
R_93, 
R_94, 
R_95, 
R_96, 
R_97, 
R_98, 
R_99, 
R_100)
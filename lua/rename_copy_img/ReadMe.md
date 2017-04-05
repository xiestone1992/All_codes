********************************************************************************************

How to run **********************************************

********************************************************************************************
$ th main.lua 




********************************************************************************************
********************************************************************************************

Sample:
I have some pictures in folder '/data' .
There are 3 sub-class folders in '/data', such as: fish, flower and tv.
I only want to select some certain images which must fit following requirements:

Image size :	1980x1289 
Channels: 	3
Rename them with folder name and copy them into the dstination folder with sub-class folder.

so that I modify parameters in main.lua in line 153:

  main('data',  'result' ,        3             , 1           ,1980,1289)

********************************************************************************************
********************************************************************************************
--[src] : 		source folder, contains sub-folder(raw data) 
--[dst] : 		destination folder  
--[color_or_gray] : 	'0' select all images, '1' for gray image,    '3' only for color image  
--[opt_folder]    : 	'1' for sub-classes folder, '0' for single folder  
--[output_img_W] :      select images with certain width, '0' for select all images  
--[output_img_H] :	select images with certain height,  '0' for select all iamges  

--main([src],[dst],[color_or_gray] ,[opt_folder],[output_img_W],[output_img_H] )

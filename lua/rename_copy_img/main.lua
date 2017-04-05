
--[[
Find, rename and copy files(images) to Target folder
--]]

require 'image'



local function channel_and_folder(opt_channel,opt_folder)
  -- Color for Gray images

  if (opt_channel == 0) then print('Images type:\tGray & Color Images ') end
  if (opt_channel == 1) then print('Images type:\tGray Images ') end
  if (opt_channel == 3) then print('Images type:\tColor Images ') end

  -- Single folders or sub-classes folders
  if (opt_folder == 1) then 
    print('Folder type:\tSub-class folder ')
  else
    print('Folder type:\tOne single folder ')
  end

end

local function show_input_parameters(srcFolder,dstFolder,opt_channel,opt_folder,set_W,set_H)

  print('\nSrcFolder is:\t'..srcFolder)
  print('DstFolder is:\t'..dstFolder)
  channel_and_folder(opt_channel,opt_folder)
  if(set_W ~= 0)and(set_H~=0)then
    print('Image size :\t'..set_W..'x'..set_H)
  end

end

local function count_number(foldIdx,count_img,folder_num,sum_img)

    print('# '..folder_num..', '..foldIdx..' folder, there are '..count_img..' Images')
    sum_img = sum_img + count_img
    folder_num = folder_num + 1 
  return sum_img,folder_num

end

local function check_dst(dstFolder)

   local dstDirectory = paths.concat(paths.cwd(), dstFolder) -- 1st dstFolder
   if  paths.dirp(dstDirectory) then -- Check dstDir, make sure that following operations will not recover previous data     
    print('\n\n\n                "'..dstFolder..'" folder exist !!  \n                Do NOT cover previous Data !!\n\n\n')
    os.exit()
   end

end

local function check_src(srcDirectory)

     -- Check srcDir
     if not paths.dirp(srcDirectory) then 
       print('\n\n\n                  Need the source folder!\n\n\n')
       os.exit() 
     end 

end

local function copy_img (img_H,img_W,set_H,set_W,count_img,copy_img_file,new_name_path,img_file_path)
    if (img_H == set_H) and (img_W == set_W) then -- select image with certain width & height
      os.execute('cp '..img_file_path..' '..new_name_path..'_'..count_img..'.jpg')
      count_img = count_img + 1 
    end

    if (set_H == 0 ) and ( set_W == 0) then -- select image without certain width & height
      os.execute('cp '..img_file_path..' '..new_name_path..'_'..count_img..'.jpg')
      count_img = count_img + 1 
    end 

  return count_img
end


local function FindImg (set_H,set_W,count_img,curDir,fileIdx,dstFolder,foldIdx,opt_channel,opt_folder)
  
  --Load Image
  local img_file_path = paths.concat(curDir, fileIdx)
  local img_file = image.load(img_file_path)
  local img_channel = img_file:size(1)
  local img_H,img_W = img_file:size(2),img_file:size(3)
  local tarDir = paths.concat(dstFolder, foldIdx)  -- 2nd dstDir,For copy image to target directory
  local new_name_path = nil
  local new_folder = nil

  local final_image = nil 

  -- Rename file(image) with lables.
  if (opt_folder == 1) then
    new_name_path = paths.concat(tarDir,foldIdx)
    new_folder = tarDir
  else
    new_name_path = tarDir
    new_folder = dstFolder
  end

  -- Create folder
  if not paths.dirp(new_folder) then paths.mkdir(new_folder) end

  --Select certain Images
  if (opt_channel == 0 ) then  -- Select gray & color image
    final_image = copy_img_file
  elseif (img_channel == opt_channel) then  -- Select gray or color image
    final_image = copy_img_file
  end

  --Copy images
  count_img = copy_img(img_H,img_W,set_H,set_W,count_img,final_image,new_name_path,img_file_path)
  return count_img
end

local function main(srcFolder,dstFolder,opt_channel,opt_folder,set_W,set_H)
  
  -- Show input info.
  show_input_parameters(srcFolder,dstFolder,opt_channel,opt_folder,set_W,set_H)

  local srcDirectory = paths.concat(paths.cwd(), srcFolder) -- get 1st srcFolder path
  local folder_num = 0
  local sum_img = 0  

  -- Check src & dst
  check_src(srcDirectory) -- Check srcDir exist or not
  check_dst(dstFolder) -- Check dstDir exist or not

  for foldIdx in paths.iterdirs(srcDirectory) do -- Enter 1st Folder
    print('\n+++  # '..folder_num..'  ++++++++++++++++++++++++++++  In folder '..foldIdx..'  +++++++++++++++++++++++++++++++')
    local count_img = 0 -- count image number
    local curDir= paths.concat(srcDirectory, foldIdx) -- get image path

    for fileIdx in paths.iterfiles(curDir) do   -- do operation for single image
      count_img = FindImg(set_H,set_W,count_img,curDir,fileIdx,dstFolder,foldIdx,opt_channel,opt_folder)   -- call function 
    end
    sum_img,folder_num = count_number(foldIdx,count_img,folder_num,sum_img) -- count
  end
  print('\n\n Sum: '..sum_img..' images have been processed')
end


--[src] :     source folder, contains sub-folder(raw data)
--[dst] :     destination folder
--[color_or_gray] :   '0' select all images, '1' for gray image,    '3' only for color image
--[opt_folder]    :   '1' for sub-classes folder, '0' for single folder
--[output_img_W] :      select images with certain width 
--[output_img_H] :  select images with certain height 

--main([src],[dst],[color_or_gray] ,[opt_folder],[output_img_W],[output_img_H] )
  main('data',  'result' ,        3             , 1           ,1980,1289)

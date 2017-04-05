--[[
Ref: https://github.com/torch/demos/blob/master/load-data/load-images.lua



Show all exif info
Show certain exif info

--]]
require 'torch'
require 'xlua'
require 'image'
------------------------------------------------------
------------------------------------------------------
-- {1. Parse command-line arguments}
------------------------------------------------------
------------------------------------------------------
op = xlua.OptionParser('load-images.lua [options]')
op:option{'-d', '--dir', action='store', dest='dir', help='directory to load', req=ture}
op:option{'-e', '--ext', action='store', dest='ext', help='only load files of this extension', default='jpg'}
opt = op:parse()
op:summarize()

if (opt.dir == nil) then
	print('\n\n\n  Enter "th see_exif.lua -d /dir/you/want " \n\n\n')
end

------------------------------------------------------
------------------------------------------------------
-- {2. Load files in directory}
------------------------------------------------------
------------------------------------------------------
files = {}
for file in paths.files(opt.dir) do
	if file:find(opt.ext..'$') then  -- only load files matchs the extension, like '.jpg'
		table.insert(files,paths.concat(opt.dir,file)) -- store in table
	end
end

print('\n\nThere are '..#files..' files \n\n ')

-- {Check files}
if #files == 0 then
	error('There are not any kind of file with type: '..opt.ext)
end

------------------------------------------------------
------------------------------------------------------
-- {3. Sort file name}
------------------------------------------------------
------------------------------------------------------
table.sort(files, function(a,b) return a < b end)

------------------------------------------------------
------------------------------------------------------
-- {4. Load Images}
------------------------------------------------------
------------------------------------------------------
images = {}
for i, file in ipairs(files) do 
	--load images
	table.insert(images, image.load(file))
end
print('\n')

------------------------------------------------------
------------------------------------------------------
-- {5. Do some operations}
------------------------------------------------------
------------------------------------------------------
gm = require 'graphicsmagick'

-- Exif mode
-- '1' for all exif info
-- '0' for certain exif info
local exif_mode = 1

------ if the exif_mode = 0
-- Different value of exif_opt to show different exif info
-- '1' for ISO
-- '2' for Exposure Time
-- '3' for Aperture Value
local exif_opt = 1

if (exif_mode == 1) then -- Show all exif info
	for i = 1, #files do 
		-- Get Image size
		local img_height, img_width = images[i]:size(2), images[i]:size(3)

		-- Get Image name
		local img_name = paths.basename(files[i])
		print(img_name)

		-- Get Image exif
		local all_exif = gm.info(files[i])
		
		print(all_exif)
	end		
end

if (exif_mode == 0) then -- Show certain exif info

	for exif_opt = 1, 3 do 
		if (exif_opt == 1) then
			print('Name                Index        ISO              size')
		end

		if (exif_opt == 2) then
			print('Name                Index      Exposure Time      size')
		end

		if (exif_opt == 3) then
			print('Name                Index      Aperture Value        size')
		end

		for i = 1, #files do 

			-- Get Image size
			local img_height, img_width = images[i]:size(2), images[i]:size(3)

			-- Get Image name
			local img_name = paths.basename(files[i])

			-- Get Image exif
			local all_exif = gm.info(files[i])

			-- Select and show certain exif
			--ISO
			if (exif_opt == 1) then
				local show_exif = all_exif.exif.ISOSpeedRatings
				print(img_name..'    #'..i..'          '..show_exif..'           '..img_height..'x'..img_width)
			end

			--Exposure Time
			if (exif_opt == 2) then
				local show_exif = all_exif.exif.ExposureTime
				print(img_name..'    #'..i..'          '..show_exif..'           '..img_height..'x'..img_width)
			end

			--Aperture Value
			if (exif_opt == 3) then
				local show_exif = all_exif.exif.ApertureValue
				print(img_name..'    #'..i..'          '..show_exif..'           '..img_height..'x'..img_width)
			end
		end
		print('\n*********************************************************** \n ')
	end
end




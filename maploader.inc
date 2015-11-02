/*
* ================
* Map loader/unloader include (maploader.inc)
* The easiest and best way to load huge map codes from files, support settings, materials, textures, all dynamic items and comments!
* ================
*
* ================
* Create by: Gammix
* ================
*
* ================
* Special thanks to: Southclaw (thanks for his Map Parser filterscript), Incognito (streamer plugin), Y_Less (sscanf plugin)
* ================
*
* ================
* Functions/Stocks:
	native Map_Load(const mapname[]);
	native bool:Map_Unload(const mapname[]);
	native Map_CountItems(const mapname[], type);
	native Map_GetItemId(const mapname[], slot);
	native Map_GetItemType(const mapname[], slot);
* ================
*
*/

#if defined maploaderincluded
	#endinput
#endif
#define maploader_included

#include <streamer>
#include <sscanf2>

#if ! defined MAX_MAPS
	#define MAX_MAPS (20)
#endif

#if ! defined MAX_MAP_ITEMS
	#define MAX_MAP_ITEMS (5000)
#endif

stock static
			mapNames[MAX_MAPS][35],
			mapLoadedCount[MAX_MAPS],
			mapLoadedItems[MAX_MAPS][MAX_MAP_ITEMS],
			mapLoadedItemsType[MAX_MAPS][MAX_MAP_ITEMS]
;

stock Map_Load(const mapname[])
{
	if (! fexist(mapname))
	{
	    return 0;
 	}

	new
	    index = -1
	;
	for (new i; i < MAX_MAPS; i++)
	{
	    if (mapNames[i][0] && ! strcmp(mapNames[i], mapname))
		{
		    index = i;

			return mapLoadedCount[i];
		}
		else if (! mapNames[i][0] && index == -1)
		{
		    index = i;
		}
	}

	if (index == -1)
 	{
	 	return 0;
	}

	new
	    File:file = fopen(mapname, io_read)
	;
	if (! file)
	{
	    return 0;
 	}

 	new
				line[256],
				linenumber = 1,
				funcname[32],
				funcargs[156]
	;

	new
				modelid,
		Float:	x,
		Float:	y,
		Float:	z,
		Float:	rx,
		Float:	ry,
		Float:	rz,
				worldid,
				interiorid,
				playerid,
		Float:	streamdistance,
		Float:	drawdistance,
                type,
		Float:  size,
		        attachplayerid,
				attachvehicleid,
		        testlos,
		        idx,
				text[128],
				resname[32],
				res,
				font[32],
				fontsize,
				bold,
				fontcol,
				bgcol,
				align,
				txdname[32],
				texturename[32],
				matcol
	;

	static const materialsizes[][10] =
	{
		"32x32",
		"64x32",
		"64x64",
		"128x32",
		"128x64",
		"128x128",
		"256x32",
		"256x64",
		"256x128",
		"256x256",
		"512x64",
		"512x128",
		"512x256",
		"512x512"
	};

	while (fread(file, line))
	{
		if (line[0] < 65)
		{
			linenumber++;
			continue;
		}

	    if (sscanf(line, "p<(>s[32]p<)>s[156]{s[96]}", funcname, funcargs))
	    {
			linenumber++;
	        continue;
	    }

		// objects
		if (! strcmp(funcname, "CreateDynamicObject", true))
		{
			if (! sscanf(funcargs, "p<,>dffffffD(-1)D(-1)D(-1)F("#STREAMER_OBJECT_SD")F("#STREAMER_OBJECT_DD")", modelid, x, y, z, rx, ry, rz, worldid, interiorid, playerid, streamdistance, drawdistance))
			{
			    mapLoadedCount[index]++;
				mapLoadedItems[index][mapLoadedCount[index]] = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, worldid, interiorid, playerid, streamdistance, drawdistance);
				mapLoadedItemsType[index][mapLoadedCount[index]] = STREAMER_TYPE_OBJECT;
			}
		}


		if (! strcmp(funcname, "SetDynamicObjectMaterialText", true))
		{
			if (! sscanf(funcargs, "p<,>{s[32]} d p<\">{s[2]}s[32]p<,>{s[2]} s[32] p<\">{s[2]}s[32]p<,>{s[2]} ddxxd", idx, text, resname, font, fontsize, bold, fontcol, bgcol, align))
			{
				new
					len = strlen(text)
				;

				res = strval(resname[0]);

				if (res == 0)
				{
					for (new i, j = sizeof(materialsizes); i < j; i++)
					{
						if (strfind(resname, materialsizes[i]) != -1)
						{
							res = (i + 1) * 10;
						}
					}
				}

				for (new i; i < len; i++)
				{
					if (text[i] == '\\' && i != len - 1)
					{
						if (text[i + 1] == 'n')
						{
							strdel(text, i, i + 1);
							text[i] = '\n';
						}
					}
				}

				SetDynamicObjectMaterialText(mapLoadedItems[index][mapLoadedCount[index]], idx, text, res, font, fontsize, bold, fontcol, bgcol, align);
			}
		}

		if (! strcmp(funcname, "SetDynamicObjectMaterial", true))
		{
			if (! sscanf(funcargs, "p<,>{s[16]}dd p<\">{s[1]}s[32]p<,>{s[1]} p<\">{s[1]}s[32]p<,>{s[1]} x", idx, modelid, txdname, texturename, matcol))
			{
				SetDynamicObjectMaterial(mapLoadedItems[index][mapLoadedCount[index]], idx, modelid, txdname, texturename, matcol);
			}
		}

		// pickups
		if (! strcmp(funcname, "CreateDynamicPickup", true))
		{
			if (! sscanf(funcargs, "p<,>ddfffD(-1)D(-1)D(-1)F("#STREAMER_PICKUP_SD")", modelid, type, x, y, z, worldid, interiorid, playerid, streamdistance))
			{
			    mapLoadedCount[index]++;
				mapLoadedItems[index][mapLoadedCount[index]] = CreateDynamicPickup(modelid, type, x, y, z, worldid, interiorid, playerid, streamdistance);
				mapLoadedItemsType[index][mapLoadedCount[index]] = STREAMER_TYPE_PICKUP;
			}
		}

		// checkpoint
		if (! strcmp(funcname, "CreateDynamicCP", true))
		{
			if (! sscanf(funcargs, "p<,>ffffD(-1)D(-1)D(-1)F("#STREAMER_CP_SD")", x, y, z, size, worldid, interiorid, playerid, streamdistance))
			{
			    mapLoadedCount[index]++;
				mapLoadedItems[index][mapLoadedCount[index]] = CreateDynamicCP(x, y, z, size, worldid, interiorid, playerid, streamdistance);
				mapLoadedItemsType[index][mapLoadedCount[index]] = STREAMER_TYPE_CP;
			}
		}

		// race checkpoint
		if (! strcmp(funcname, "CreateDynamicRaceCP", true))
		{
			if (! sscanf(funcargs, "p<,>dfffffffD(-1)D(-1)D(-1)F("#STREAMER_RACE_CP_SD")", type, x, y, z, rx, ry, rz, size, worldid, interiorid, playerid, streamdistance))
			{
			    mapLoadedCount[index]++;
				mapLoadedItems[index][mapLoadedCount[index]] = CreateDynamicRaceCP(type, x, y, z, rx, ry, rz, size, worldid, interiorid, playerid, streamdistance);
				mapLoadedItemsType[index][mapLoadedCount[index]] = STREAMER_TYPE_RACE_CP;
			}
		}

		// mapicon
		if (! strcmp(funcname, "CreateDynamicMapIcon", true))
		{
			if (! sscanf(funcargs, "p<,>fffddD(-1)D(-1)D(-1)F("#STREAMER_MAPICON_SD")D("#MAPICON_LOCAL")", x, y, z, type, fontcol, worldid, interiorid, playerid, streamdistance, res))
			{
			    mapLoadedCount[index]++;
				mapLoadedItems[index][mapLoadedCount[index]] = CreateDynamicMapIcon(x, y, z, type, fontcol, worldid, interiorid, playerid, streamdistance, res);
				mapLoadedItemsType[index][mapLoadedCount[index]] = STREAMER_TYPE_MAP_ICON;
			}
		}

		// 3d text label
		if (! strcmp(funcname, "CreateDynamic3DTextLabel", true))
		{
			if (! sscanf(funcargs, "p<,>{s[32]} dffffD(-1)D(-1)D(0)D(-1)D(-1)D(-1)D(-1)F("#STREAMER_3D_TEXT_LABEL_SD")", text, fontcol, x, y, z, drawdistance, attachplayerid, attachvehicleid, testlos, worldid, interiorid, playerid, streamdistance))
			{
			    mapLoadedCount[index]++;
				mapLoadedItems[index][mapLoadedCount[index]] = _:CreateDynamic3DTextLabel(text, fontcol, x, y, z, drawdistance, attachplayerid, attachvehicleid, testlos, worldid, interiorid, playerid, streamdistance);
				mapLoadedItemsType[index][mapLoadedCount[index]] = STREAMER_TYPE_3D_TEXT_LABEL;
			}
		}

		linenumber++;

		if (mapLoadedCount[index] > MAX_MAP_ITEMS)
		{
			break;
		}
	}

	fclose(file);
	
	strcat(mapNames[index], mapname);

	return mapLoadedCount[index];
}

stock bool:Map_Unload(const mapname[])
{
	new
	    index = -1
	;
	for (new i; i < MAX_MAPS; i++)
	{
	    if (mapNames[i][0] && ! strcmp(mapNames[i], mapname))
		{
		    index = i;

		    if (! mapLoaded[i])
		    {
		    	return false;
			}

		    break;
		}
	}

	if (index == -1)
 	{
	 	return false;
	}

 	for (new i, j = mapLoadedCount[index]; i < j; i++)
 	{
 		DestroyDynamicObject(mapLoadedItems[index][i]);
 		mapLoadedItems[index][i] = -1;
 		mapLoadedItemsType[index][i] = -1;
 	}

	mapNames[index][0] = EOS;
	mapLoaded[index] = false;
 	mapLoadedCount[index] = 0;

 	return true;
}

stock Map_CountItems(const mapname[], type)
{
	new
	    index = -1
	;
	for (new i; i < MAX_MAPS; i++)
	{
	    if (mapNames[i][0] && ! strcmp(mapNames[i], mapname))
		{
		    index = i;

		    break;
		}
	}

	if (index == -1)
 	{
	 	return false;
	}

	new
	    count
	;
 	for (new i, j = mapLoadedCount[index]; i < j; i++)
 	{
 		if (mapLoadedItemsType[index][i] == type)
 		{
 		    count++;
 		}
 	}

 	return count;
}

stock Map_GetItemId(const mapname[], slot)
{
	new
	    index = -1
	;
	for (new i; i < MAX_MAPS; i++)
	{
	    if (mapNames[i][0] && ! strcmp(mapNames[i], mapname))
		{
		    index = i;

		    break;
		}
	}

	if (index == -1)
 	{
	 	return false;
	}

	new
	    count
	;

	return mapLoadedItems[index][slot];
}

stock Map_GetItemType(const mapname[], slot)
{
	new
	    index = -1
	;
	for (new i; i < MAX_MAPS; i++)
	{
	    if (mapNames[i][0] && ! strcmp(mapNames[i], mapname))
		{
		    index = i;

		    break;
		}
	}

	if (index == -1)
 	{
	 	return false;
	}

	new
	    count
	;

	return mapLoadedItemsType[index][slot];
}

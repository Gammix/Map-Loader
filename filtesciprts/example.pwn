#include <a_samp>
#include <maploader>

LoadMapFromFile(const mapname[])
{
	new
	    items
	;
    print("\n========================\n");
    printf("Attempting to load map: \"%s\"...", mapname);
    print("----------");
    items = Map_Load(mapname);
    if (! items)
    {
	    print("Due to some problem, the map file couldn't be loaded!");
	    print("----------");
	    print("Loading finished with an ERROR!");
	    print("\n========================\n");
    }
    else
    {
	    printf("Map file initiated with \"%i\" items.", items);
	    printf("* Objects: %i", Map_CountItems(mapname, STREAMER_TYPE_OBJECT));
	    printf("* Picups: %i", Map_CountItems(mapname, STREAMER_TYPE_PICKUP));
	    printf("* Checkpoints: %i", Map_CountItems(mapname, STREAMER_TYPE_CP));
	    printf("* Race Checkpoints: %i", Map_CountItems(mapname, STREAMER_TYPE_RACE_CP));
	    printf("* Mapicons: %i", Map_CountItems(mapname, STREAMER_TYPE_MAP_ICON));
	    printf("* 3D Text Labels: %i", Map_CountItems(mapname, STREAMER_TYPE_3D_TEXT_LABEL));
	    print("----------");
	    print("Loading finished!");
	    print("\n========================\n");
    }
}

public OnFilterScriptInit
{
    LoadMapFromFile("map1.ini");
    LoadMapFromFile("map2.ini");
}

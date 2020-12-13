/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/system.h>
#include <engine/shared/datafile.h>
#include <engine/storage.h>
#include <game/gamecore.h>
#include <game/mapitems.h>

int main(int argc, const char **argv)
{
	dbg_logger_stdout();

	IStorage *pStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_BASIC, argc, argv);
	int Index, ID = 0, Type = 0, Size;
	void *pPtr;
	char aFileName[1024];
	CDataFileReader DataFile;
	CDataFileWriter df;

	if(!pStorage || argc != 3)
	{
		dbg_msg("map_update_fng", "usage: %s input.map output.map", argv[0]);
		return -1;
	}

	str_format(aFileName, sizeof(aFileName), "%s", argv[2]);

	if(!DataFile.Open(pStorage, argv[1], IStorage::TYPE_ABSOLUTE))
	{
		dbg_msg("map_update_fng", "error opening input map '%s'", argv[1]);
		return -1;
	}
	if(!df.Open(pStorage, aFileName, IStorage::TYPE_ABSOLUTE))
	{
		dbg_msg("map_update_fng", "error opening output map '%s'", aFileName);
		return -1;
	}

	char aGame[16];
	str_format(aGame, sizeof(aGame), "Game");

	// add all items
	for(Index = 0; Index < DataFile.NumItems(); Index++)
	{
		pPtr = DataFile.GetItem(Index, &Type, &ID);
		Size = DataFile.GetItemSize(Index);

		// update outdated index
		if(Type == MAPITEMTYPE_LAYER)
		{
			CMapItemLayerTilemap *pTileMap = (CMapItemLayerTilemap *)pPtr;
			char aName[16];
			IntsToStr(pTileMap->m_aName, sizeof(pTileMap->m_aName) / sizeof(int), aName);

			if(str_comp(aGame, aName) == 0)
			{
				CTile *pTiles = (CTile *)DataFile.GetData(pTileMap->m_Data);

				for(int y = 0; y < pTileMap->m_Height; y++)
				{
					for(int x = 0; x < pTileMap->m_Width; x++)
					{
						int Index = pTiles[y * pTileMap->m_Width + x].m_Index;
						if(Index >= 208 && Index <= 210)
						{
							pTiles[y * pTileMap->m_Width + x].m_Index -= 200;
						}
					}
				}
			}
		}

		df.AddItem(Type, ID, Size, pPtr);
	}

	// add all data
	for(Index = 0; Index < DataFile.NumData(); Index++)
	{
		pPtr = DataFile.GetData(Index);
		Size = DataFile.GetDataSize(Index);
		df.AddData(Size, pPtr);
	}

	DataFile.Close();
	df.Finish();

	dbg_msg("map_update_fng", "Done");
	return 0;
}

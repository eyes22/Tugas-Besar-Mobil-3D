/**
    Keterangan   : Tugas Besar Mata Kuliah Grafika Komputer
    Nama Program : Mobil 3D OpenGL
    Kelompok 2
    Anggota Kelompok : 1. Wanda             10108419
                       2. Giri Supangkat    10108423
                       3. Ringga Anggiat S  10107569
                       4. Muzakki Fadhil    10108474
*/

#ifndef MILKSHAPEMODEL_H
#define MILKSHAPEMODEL_H

#include "Model.h"

class MilkshapeModel : public Model
{
	public:
		/*	Constructor. */
		MilkshapeModel();

		/*	Destructor. */
		virtual ~MilkshapeModel();

		/*
			Load the model data into the private variables.
				filename			Model filename
		*/
		virtual bool loadModelData( const char *filename );
};

#endif // ndef MILKSHAPEMODEL_H

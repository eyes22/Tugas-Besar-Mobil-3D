/**
    Keterangan   : Tugas Besar Mata Kuliah Grafika Komputer
    Nama Program : Mobil 3D OpenGL
    Kelompok 2
    Anggota Kelompok : 1. Wanda             10108419
                       2. Giri Supangkat    10108423
                       3. Ringga Anggiat S  10107569
                       4. Muzakki Fadhil    10108474
*/

#ifndef MODEL_H
#define MODEL_H

class Model
{
	public:
		//	Mesh
		struct Mesh
		{
			int m_materialIndex;
			int m_numTriangles;
			int *m_pTriangleIndices;
		};

		//	properties material
		struct Material
		{
			float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
			float m_shininess;
			GLuint m_texture;
			char *m_pTextureFilename;
		};

		//	Struktur triangle
		struct Triangle
		{
			float m_vertexNormals[3][3];
			float m_s[3], m_t[3];
			int m_vertexIndices[3];
		};

		//	struktur vertex
		struct Vertex
		{
			char m_boneID;	// for skeletal animation
			float m_location[3];
		};

	public:
		/*	Constructor. */
		Model();

		/*	Destructor. */
		virtual ~Model();

		/*
			Load the model data into the private variables.
				filename			Model filename
		*/
		virtual bool loadModelData( const char *filename ) = 0;

		/*
			membuat model
		*/
		void draw();

		/*
			dipanggil jika OpenGL context telah hilang  dan kita butuh reload textures, display lists, etc.
		*/
		void reloadTextures();

	protected:
		//	Meshes used
		int m_numMeshes;
		Mesh *m_pMeshes;

		//	Materials used
		int m_numMaterials;
		Material *m_pMaterials;

		//	Triangles used
		int m_numTriangles;
		Triangle *m_pTriangles;

		//	Vertices Used
		int m_numVertices;
		Vertex *m_pVertices;
};

#endif // ndef MODEL_H

#CCFLAGS=-Wall -fPIC -g -I "./" -I/home/dan/Desktop/wxGTK-2.8.9/buildgtk/lib/wx/include/gtk2-ansi-debug-2.8 -I/usr/include/wx-2.8 -I$(HEEKSCADPATH) -D__WXGTK__ -DHEEKSART -D__WXDEBUG__ -DWXUSINGDLL
CCFLAGS=-Wall -fPIC -g -I "./" -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -I$(HEEKSCADPATH) -D__WXGTK__ -DHEEKSART -DUNICODE -DWXUSINGDLL -DTIXML_USE_STL

#WX_LIB_LETTER=d
WX_LIB_LETTER=u

OBJECTS=HeeksArt.o HeeksColor.o HeeksObj.o Interface.o Matrix.o Mesh.o MeshChild.o MeshEdge.o MeshFace.o MeshPosition.o MeshVertex.o ObjList.o Plane.o PropertyCheck.o PropertyChoice.o PropertyColor.o PropertyDouble.o PropertyInt.o PropertyList.o PropertyString.o PropertyVertex.o tinyxml.o tinyxmlerror.o tinyxmlparser.o ToolImage.o TransientObject.o

SHARED_LIBRARY=./libheeksart.so.0.1.0

${SHARED_LIBRARY}: ${OBJECTS}
	gcc -shared -fPIC -Wl,-soname,libheeksart.so.0 -o ${SHARED_LIBRARY} ${OBJECTS} -lstdc++ -lGLU -lGL -lwx_gtk2${WX_LIB_LETTER}_richtext-2.8 -lwx_gtk2${WX_LIB_LETTER}_aui-2.8 -lwx_gtk2${WX_LIB_LETTER}_xrc-2.8 -lwx_gtk2${WX_LIB_LETTER}_qa-2.8 -lwx_gtk2${WX_LIB_LETTER}_html-2.8 -lwx_gtk2${WX_LIB_LETTER}_adv-2.8 -lwx_gtk2${WX_LIB_LETTER}_core-2.8 -lwx_base${WX_LIB_LETTER}_xml-2.8 -lwx_base${WX_LIB_LETTER}_net-2.8 -lwx_base${WX_LIB_LETTER}-2.8 -lwx_gtk2${WX_LIB_LETTER}_gl-2.8

HeeksArt.o: HeeksArt.cpp
	gcc -c $? ${CCFLAGS} -o $@

HeeksColor.o: $(HEEKSCADPATH)/interface/HeeksColor.cpp
	gcc -c $? ${CCFLAGS} -o $@

HeeksObj.o: $(HEEKSCADPATH)/interface/HeeksObj.cpp
	gcc -c $? ${CCFLAGS} -o $@

Interface.o: Interface.cpp
	gcc -c $? ${CCFLAGS} -o $@

Matrix.o: Matrix.cpp
	gcc -c $? ${CCFLAGS} -o $@

Mesh.o: Mesh.cpp
	gcc -c $? ${CCFLAGS} -o $@

MeshChild.o: MeshChild.cpp
	gcc -c $? ${CCFLAGS} -o $@

MeshEdge.o: MeshEdge.cpp
	gcc -c $? ${CCFLAGS} -o $@

MeshFace.o: MeshFace.cpp
	gcc -c $? ${CCFLAGS} -o $@

MeshPosition.o: MeshPosition.cpp
	gcc -c $? ${CCFLAGS} -o $@

MeshVertex.o: MeshVertex.cpp
	gcc -c $? ${CCFLAGS} -o $@

ObjList.o: $(HEEKSCADPATH)/interface/ObjList.cpp
	gcc -c $? ${CCFLAGS} -o $@

Plane.o: Plane.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyCheck.o: $(HEEKSCADPATH)/interface/PropertyCheck.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyChoice.o: $(HEEKSCADPATH)/interface/PropertyChoice.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyColor.o: $(HEEKSCADPATH)/interface/PropertyColor.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyDouble.o: $(HEEKSCADPATH)/interface/PropertyDouble.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyInt.o: $(HEEKSCADPATH)/interface/PropertyInt.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyList.o: $(HEEKSCADPATH)/interface/PropertyList.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyString.o: $(HEEKSCADPATH)/interface/PropertyString.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyVertex.o: $(HEEKSCADPATH)/interface/PropertyVertex.cpp
	gcc -c $? ${CCFLAGS} -o $@

tinyxml.o: $(HEEKSCADPATH)/tinyxml/tinyxml.cpp
	gcc -c $? ${CCFLAGS} -o $@

tinyxmlerror.o: $(HEEKSCADPATH)/tinyxml/tinyxmlerror.cpp
	gcc -c $? ${CCFLAGS} -o $@

tinyxmlparser.o: $(HEEKSCADPATH)/tinyxml/tinyxmlparser.cpp
	gcc -c $? ${CCFLAGS} -o $@

ToolImage.o: $(HEEKSCADPATH)/interface/ToolImage.cpp
	gcc -c $? ${CCFLAGS} -o $@

TransientObject.o: $(HEEKSCADPATH)/interface/TransientObject.cpp
	gcc -c $? ${CCFLAGS} -o $@

clean:
	-rm ${SHARED_LIBRARY} ${OBJECTS}



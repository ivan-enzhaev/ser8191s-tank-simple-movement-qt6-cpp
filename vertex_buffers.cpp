#include "vertex_buffers.h"

#include <QtCore/QFile>
#include <QtGui/QMatrix4x4>
#include <QtXml/QDomDocument>

VertexBuffers::VertexBuffers() { }

VertexBuffersData VertexBuffers::init(const QString &path)
{
    VertexBuffersData buffers;
    QDomDocument xmlDoc;
    QFile f(path);
    if (!f.open(QIODevice::OpenModeFlag::ReadOnly))
    {
        qDebug() << "Failed to open file: " + path;
        return buffers;
    }
    xmlDoc.setContent(&f);
    f.close();
    QDomElement root = xmlDoc.documentElement();
    QDomElement daeElem = root.firstChildElement();
    QString strPositions, strNormals, strTexCoords, strAllIndices;
    while (!daeElem.isNull())
    {
        if (daeElem.tagName() == "library_geometries")
        {
            QDomElement geomElem = daeElem.firstChildElement();
            if (geomElem.tagName() == "geometry")
            {
                QDomElement meshElem = geomElem.firstChildElement();
                if (meshElem.tagName() == "mesh")
                {
                    QDomElement meshChildElem = meshElem.firstChildElement();
                    while (!meshChildElem.isNull())
                    {
                        if (meshChildElem.attribute("id").endsWith("-mesh-positions"))
                        {
                            QDomElement floatArrayElem = meshChildElem.firstChildElement();
                            strPositions = floatArrayElem.firstChild().toText().data();
                        }
                        if (meshChildElem.attribute("id").endsWith("-mesh-normals"))
                        {
                            QDomElement floatArrayElem = meshChildElem.firstChildElement();
                            strNormals = floatArrayElem.firstChild().toText().data();
                        }
                        if (meshChildElem.attribute("id").endsWith("-mesh-map-0"))
                        {
                            QDomElement floatArrayElem = meshChildElem.firstChildElement();
                            strTexCoords = floatArrayElem.firstChild().toText().data();
                        }
                        if (meshChildElem.tagName() == "triangles" || meshChildElem.tagName() == "polylist")
                        {
                            QDomElement trianglesChildElem = meshChildElem.firstChildElement();
                            while (!trianglesChildElem.isNull())
                            {
                                if (trianglesChildElem.tagName() == "p")
                                {
                                    strAllIndices = trianglesChildElem.firstChild().toText().data();
                                }
                                trianglesChildElem = trianglesChildElem.nextSiblingElement();
                            }
                        }
                        meshChildElem = meshChildElem.nextSiblingElement();
                    }
                }
            }
        }
        daeElem = daeElem.nextSiblingElement();
    }

    QStringList dataVertPosList = strPositions.split(" ");
    float dataVertPosArray[dataVertPosList.length()];
    for (int i = 0; i < dataVertPosList.length(); i++)
    {
        dataVertPosArray[i] = dataVertPosList[i].toFloat();
    }

    QStringList dataNormalList = strNormals.split(" ");
    float dataNormalArray[dataNormalList.length()];
    for (int i = 0; i < dataNormalList.length(); i++)
    {
        dataNormalArray[i] = dataNormalList[i].toFloat();
    }

    QStringList dataTexCoordList = strTexCoords.split(" ");
    float dataTexCoordArray[dataTexCoordList.length()];
    for (int i = 0; i < dataTexCoordList.length(); i++)
    {
        dataTexCoordArray[i] = dataTexCoordList[i].toFloat();
    }

    QStringList dataIndexList = strAllIndices.split(" ");
    float dataIndexArray[dataIndexList.length()];
    for (int i = 0; i < dataIndexList.length(); i++)
    {
        dataIndexArray[i] = dataIndexList[i].toFloat();
    }

    int numAttributes = 3;
    float vertPositions[3 * dataIndexList.length() / numAttributes];
    float normals[3 * dataIndexList.length() / numAttributes];
    float texCoords[2 * dataIndexList.length() / numAttributes];
    int vi = 0, ni = 0, ti = 0;
    QMatrix4x4 rotMatrix;
    rotMatrix.rotate(-90.f, QVector3D(1.f, 0.f, 0.f));
    for (int i = 0; i < dataIndexList.length(); i += numAttributes)
    {
        int vertPosIndex = dataIndexArray[i + 0];
        float vx = dataVertPosArray[vertPosIndex * 3 + 0];
        float vy = dataVertPosArray[vertPosIndex * 3 + 1];
        float vz = dataVertPosArray[vertPosIndex * 3 + 2];
        QVector3D oldPos(vx, vy, vz);
        QVector3D newPos = rotMatrix * oldPos;
        vertPositions[vi++] = newPos.x();
        vertPositions[vi++] = newPos.y();
        vertPositions[vi++] = newPos.z();

        int normalIndex = dataIndexArray[i + 1];
        float nx = dataNormalArray[normalIndex * 3 + 0];
        float ny = dataNormalArray[normalIndex * 3 + 1];
        float nz = dataNormalArray[normalIndex * 3 + 2];
        QVector3D oldNormal(nx, ny, nz);
        QVector3D newNormal = rotMatrix * oldNormal;
        normals[ni++] = newNormal.x();
        normals[ni++] = newNormal.y();
        normals[ni++] = newNormal.z();

        int texCoordIndex = dataIndexArray[i + 2];
        texCoords[ti++] = dataTexCoordArray[texCoordIndex * 2 + 0];
        texCoords[ti++] = dataTexCoordArray[texCoordIndex * 2 + 1];
    }

    QOpenGLBuffer vertPosBuffer;
    vertPosBuffer.create();
    vertPosBuffer.bind();
    vertPosBuffer.allocate(vertPositions, sizeof(vertPositions));

    QOpenGLBuffer normalBuffer;
    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.allocate(normals, sizeof(normals));

    QOpenGLBuffer texCoordBuffer;
    texCoordBuffer.create();
    texCoordBuffer.bind();
    texCoordBuffer.allocate(texCoords, sizeof(texCoords));

    int amountOfVertices = dataIndexList.length() / numAttributes;

    buffers.amountOfVertices = amountOfVertices;
    buffers.vertPosBuffer = vertPosBuffer;
    buffers.normalBuffer = normalBuffer;
    buffers.texCoordBuffer = texCoordBuffer;

    return buffers;
}

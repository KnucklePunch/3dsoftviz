#ifndef GITEVOLUTIONGRAPH_H
#define GITEVOLUTIONGRAPH_H

#include "GitLib/GitExport.h"
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

namespace Repository {
namespace Git {
class GitVersion;

class GITLIB_EXPORT GitEvolutionGraph
{
public:
	/**
	 * Explicit default constructor
	 * @brief Creates blank evolution graph
	 */
	GitEvolutionGraph();

	~GitEvolutionGraph();

	/**
	 * Explicit default constructor
	 * @brief Creates evolution graph with set up filepath
	 * @param filePath Path to the git repository on file system
	 */
	GitEvolutionGraph( QString filePath );

	/**
	 * void setVersions( QList<Repository::Git::GitVersion*> versions )
	 * @brief Setter of list of versions
	 * @param versions list of versions
	 */
	void setVersions( QList<Repository::Git::GitVersion*> versions )
	{
		this->versions = versions;
	}

	/**
	 * QList<Repository::Git::GitVersion*> getVersions()
	 * @brief Getter of list of versions
	 * @return list of versions
	 */
	QList<Repository::Git::GitVersion*> getVersions()
	{
		return this->versions;
	}

	/**
	 * void setFilePath( QString filePath )
	 * @brief Setter of path to the git repository on file system
	 * @param filePath Path to the git repository on file system
	 */
	void setFilePath( QString filePath )
	{
		this->filePath = filePath;
	}

	/**
	 * QString getFilePath()
	 * @brief Getter of path to the git repository on file system
	 * @return path to the git repository on file system
	 */
	QString getFilePath()
	{
		return this->filePath;
	}

	/**
	 * Repository::Git::GitVersion* getVersion( int position )
	 * @brief Getter of specific version in list of versions
	 * @param position Index of version in list of versions
	 * @return Specific versions from list
	 */
	Repository::Git::GitVersion* getVersion( int position )
	{
		return this->versions.at( position );
	}

	/**
	 * int getLifespan()
	 * @brief Returns lifespan of deleted files in graph
	 * @return lifespan of deleted files in graph
	 */
	int getLifespan()
	{
		return this->lifespan;
	}

	/**
	 * void setLifespan( int lifespan )
	 * @brief Sets a lifespan of deleted files in graph
	 * @param lifespan Lifespan of deleted files in graph
	 */
	void setLifespan( int lifespan )
	{
		this->lifespan = lifespan;
	}

	/**
	 * QMap<QString, int> getRemovedFiles()
	 * @brief Returns map, where key is deleted file path in project and value version, in which was deleted
	 * @return map, where key is deleted file path in project and value version, in which was deleted
	 */
	QMap<QString, int> getRemovedFiles()
	{
		return this->removedFiles;
	}

	/**
	 * void addRemovedFiles( QStringList list, int version )
	 * @brief Adds removed files with version to map
	 * @param list List of files in path
	 * @param version Version where file was deleted
	 */
	void addRemovedFiles( QStringList list, int version );

    /**
     * void addLuaNodeMapping( QString identifier, int luaNodeId )
     * @brief Add mapping of lua node indetifier to actual lua node id
     * @param identifier Unique identifier of lua node
     * @param luaNodeId Unique id of current loaded lua node
     */
    void addLuaNodesMapping( QString identifier, int luaNodeId );

    /**
     * QMap<QString, int> getLuaNodeMapping()
     * @brief Returns map, where key is unique identifier of lua node and value is unique id of current loaded lua node
     * @return map, where key is unique identifier of lua node and value is unique id of current loaded lua node
     */
    QMap<QString, int> getLuaNodesMapping() {
        return this->luaNodesMapping;
    }

    /**
     * void addLuaEdgesMapping( QString identifier, int luaEdgeId )
     * @brief Add mapping of lua edge indetifier to actual lua edge id
     * @param identifier Unique identifier of lua edge
     * @param luaNodeId Unique id of current loaded lua edge
     */
    void addLuaEdgesMapping( QString identifier, int luaEdgeeId );

    /**
     * QMap<QString, int> getLuaEdgesMapping()
     * @brief Returns map, where key is unique identifier of lua edge and value is unique id of current loaded lua edge
     * @return map, where key is unique identifier of lua edge and value is unique id of current loaded lua edge
     */
    QMap<QString, int> getLuaEdgesMapping() {
        return this->luaEdgesMapping;
    }

	/**
	 * void removeRemovedFiles( QString name )
	 * @brief Removes file from map with specific key
	 * @param name Key of the file
	 */
	void removeRemovedFiles( QString name );

	/**
	 * void printRemovedFiles()
	 * @brief Prints out all paths and versions of map
	 */
	void printRemovedFiles();

private:
	/**
	 * QList<Repository::Git::GitVersion*> versions
	 * @brief List of versions
	 */
	QList<Repository::Git::GitVersion*> versions;

	/**
	 * QMap<QString, int> removedFiles
	 * @brief Map of names of files which was deleted in Nth version
	 */
	QMap<QString, int> removedFiles;

    /**
     * QMap<QString, int> luaNodesMapping
     * @brief Maps identifier to current lua node id
     */
    QMap<QString, int> luaNodesMapping;

    /**
     * QMap<QString, int> luaEdgesMapping
     * @brief Maps identifier to current lua edge id
     */
    QMap<QString, int> luaEdgesMapping;

	/**
	 * int lifespan
	 * @brief Lifespan of deleted files in graph
	 */
	int lifespan;

	/**
	 * QString filePath
	 * @brief Path to the git repository on file system
	 */
	QString filePath;

}; // class
} // namespace
}
#endif // GITEVOLUTIONGRAPH_H

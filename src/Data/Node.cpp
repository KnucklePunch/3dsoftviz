/*!
 * ::Node.cpp
 * Projekt 3DVisual
 *
 * TODO - reload configu sa da napisat aj efektivnejsie. Pri testoch na hranach priniesol vsak podobny prepis len male zvysenie vykonu. Teraz na to
 * nemam cas, takze sa raz k tomu vratim 8)
 */

#include "Data/Node.h"
#include "Data/Graph.h"
#include "Data/Cluster.h"

#include "Util/ApplicationConfig.h"

#include <osg/Geometry>
#include <osg/Depth>
#include <osg/CullFace>
#include <osgText/FadeText>

#include <QTextStream>

#include "Data/OsgNode.h"



Data::Node::Node( qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position )
    :AbsNode(id,name,type,graph),DbNode()
{
	//konstruktor
	//scaling je potrebne na zmensenie uzla ak je vnoreny
	this->mIsFocused = false;
	this->mTargetPosition = position;
	this->currentPosition = position * Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();
    //this->inDB = false;
	this->edges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
	this->scale = scaling;
	this->setBall( NULL );
	this->setParentBall( NULL );
	this->hasNestedNodes = false;
	this->cluster = NULL;
	// Duransky start - Pociatocne nastavenie roznych cisiel vertigo rovin pre uzly
	this->numberOfVertigoPlane = id;
	// Duransky end - Pociatocne nastavenie roznych cisiel vertigo rovin pre uzly

	settings = new QMap<QString, QString>();
	//APA

	settings->insert( "Velkost","4242" );
	settings->insert( "Farba","ruzova" );
	//APA

	int pos = 0;
	int cnt = 0;

	labelText = this->name;

	while ( ( pos = labelText.indexOf( QString( " " ), pos + 1 ) ) != -1 ) {
		if ( ++cnt % 3 == 0 ) {
			labelText = labelText.replace( pos, 1, "\n" );
		}
	}

	// MERGE BEGIN
	// toto bolo u pleska/zelera
	//	this->addDrawable(createNode(this->scale, Node::createStateSet(this->type)));

	//	//vytvorenie grafickeho zobrazenia ako label
	//	this->square = createSquare(this->type->getScale(), Node::createStateSet());
	//	this->label = createLabel(this->type->getScale(), labelText);

	// toto bolo u sivaka
	this->square = createNode( this->scale * 4, OsgNode::createStateSet( this->type ) );
	this->focusedSquare = createNode( this->scale * 16, OsgNode::createStateSet( this->type ) );
	this->addDrawable( square );
	this->label = createLabel( this->type->getScale(), labelText );

	// MERGE END
	this->force = osg::Vec3f();
	this->velocity = osg::Vec3f( 0,0,0 );
	this->fixed = false;
	this->ignore = false;
	this->positionCanBeRestricted = true;
	this->removableByUser = true;
	this->selected = false;
	this->usingInterpolation = true;

	//nastavenie farebneho typu
	float r = type->getSettings()->value( "color.R" ).toFloat();
	float g = type->getSettings()->value( "color.G" ).toFloat();
	float b = type->getSettings()->value( "color.B" ).toFloat();
	float a = type->getSettings()->value( "color.A" ).toFloat();

	this->colorOfNode=osg::Vec4( r, g, b, a );
	this->setColor( colorOfNode );

	// merging Britvik: this was here
	//setDefaultColor();

	//volovar_zac
	layerID = 0;  //node is not on layer of radial layout
	radialLayout = NULL;  //node does not belong to radial layout
	//volovar_kon

}

Data::Node::~Node( void )
{
	foreach ( qlonglong i, edges->keys() ) {
		edges->value( i )->unlinkNodes();
	}
	edges->clear(); //staci to ?? netreba spravit delete/remove ??

	delete edges;
}

bool Data::Node::isFocused() const
{
	return mIsFocused;
}

void Data::Node::setIsFocused( bool value )
{
	mIsFocused = value;

	if ( value == true ) {
		this->setDrawable( 0, focusedSquare );
		setColor( osg::Vec4( 0.5f, 1.0f, 0.0f, 1.0 ) );
	}
	else {
		this->setDrawable( 0, square );
		setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0 ) );
	}
}


void Data::Node::addEdge( osg::ref_ptr<Data::Edge> edge )
{
	//pridanie napojenej hrany na uzol
	edges->insert( edge->getId(), edge );
}

void Data::Node::removeEdge( osg::ref_ptr<Data::Edge> edge )
{
	//odobranie napojenej hrany zo zoznamu
	edges->remove( edge->getId() );
}

Data::Node* Data::Node::getParentNode()
{
	//nadradeny uzol
	return this->nested_parent;
}

void Data::Node::setParentNode( Node* parent )
{
	//nastavenie nadradeneho uzla ktoremu je tento vnoreny
	this->nested_parent = parent;
}



void Data::Node::removeAllEdges()
{
	//odpojenie od vsetkych hran
	foreach ( qlonglong i, edges->keys() ) {
		edges->value( i )->unlinkNodesAndRemoveFromGraph();
	}
	edges->clear();
}

void Data::Node::showLabel( bool visible )
{
	//nastavenie zobrazenia popisku uzla
	if ( visible && !this->containsDrawable( label ) ) {
		this->addDrawable( label );
	}
	else if ( !visible ) {
		this->removeDrawable( label );
	}
}

void Data::Node::reloadConfig()
{
	this->setDrawable( 0, createNode( this->scale, OsgNode::createStateSet( this->type ) ) );
	setSelected( selected );

	osg::ref_ptr<osg::Drawable> newRect = createSquare( this->type->getScale(), OsgNode::createStateSet() );
	osg::ref_ptr<osg::Drawable> newLabel = createLabel( this->type->getScale(), labelText );

	if ( this->containsDrawable( label ) ) {
		this->setDrawable( this->getDrawableIndex( label ), newLabel );
	}

	if ( this->containsDrawable( square ) ) {
		this->setDrawable( this->getDrawableIndex( square ), newRect );
	}

	label = newLabel;
	square = newRect;
}

QSet<Data::Node*> Data::Node::getIncidentNodes() const
{
	QSet<Node*> nodes;

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i;
	for ( i = edges->begin(); i != edges->end(); ++i ) {
		osg::ref_ptr<Data::Edge> edge = i.value();
		nodes.insert( edge->getOtherNode( this ) );
	}
	return nodes;
	/*
	    if (ignoreClusters) {
	        return nodes;
	    } else {
	        QSet<Node*> visibleNodes;
	        QSetIterator<Node*> nodeIt(nodes);
	        while (nodeIt.hasNext()) {
	            Node* node = nodeIt.next();
	            Node* cluster = node->getTopCluster();
	            if (cluster != NULL && cluster != this) {
	                visibleNodes.insert(cluster);
	            } else {
	                visibleNodes.insert(node);
	            }
	        }
	        return visibleNodes;
	    }
	    */
}

void Data::Node::setDefaultColor()
{
    float r = type->getSettings()->value( "color.R" ).toFloat();
    float g = type->getSettings()->value( "color.G" ).toFloat();
    float b = type->getSettings()->value( "color.B" ).toFloat();
    float a = type->getSettings()->value( "color.A" ).toFloat();

    this->setColor( osg::Vec4( r, g, b, a ) );
}

// Duransky start - Funkcie na nastavenie a ziskanie cisla vertigo roviny, na ktorej sa uzol nachadza
void Data::Node::setNumberOfVertigoPlane( qlonglong value )
{
	this->numberOfVertigoPlane = value;
}

qlonglong Data::Node::getNumberOfVertigoPlane()
{
	return this->numberOfVertigoPlane;
}
// Duransky end - Funkcie na nastavenie a ziskanie cisla vertigo roviny, na ktorej sa uzol nachadza

QString Data::Node::toString() const
{
	QString str;
	QTextStream( &str ) << "node id:" << id << " name:" << name << " pos:[" << mTargetPosition.x() << "," << mTargetPosition.y() << "," << mTargetPosition.z() << "]";
	return str;
}

Data::Cluster* Data::Node::getCluster() const
{
	return cluster;
}

void Data::Node::setCluster( Data::Cluster* cluster )
{
	this->cluster = cluster;
}

//volovar_zac
//pre potreby odpudivych sil v radialLayoute
void Data::Node::setLayerID( int id )
{
	layerID = id;
}

int Data::Node::getLayerID()
{
	return layerID;
}

Layout::RadialLayout* Data::Node::getRadialLayout()
{
	return radialLayout;
}
void Data::Node::setRadialLayout( Layout::RadialLayout* rl )
{
	radialLayout = rl;
}

//volovar_kon

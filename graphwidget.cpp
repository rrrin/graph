/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "pch.h"
#include "graphwidget.h"
#include "edge.h"
#include "node.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>
#include<fstream>

#include <QKeyEvent>
#include <QRandomGenerator>
using namespace std;
//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
	QGraphicsScene * myscene = new QGraphicsScene(this);
	myscene->setItemIndexMethod(QGraphicsScene::NoIndex);
	myscene->setSceneRect(-200, -200, 400, 400);
    setScene(myscene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Elastic Nodes"));

	createGraph();
}
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

#if QT_CONFIG(wheelevent)
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

//! [6]
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
 
    // Text
    

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
 //   painter->setPen(Qt::lightGray);
//    painter->drawText(textRect.translated(2, 2), message);
 /*   painter->setPen(Qt::black);
    painter->drawText(textRect, message);*/
}
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
//
//  создание графа
//

typedef vector<vector<double>> mat_t;

//+варианты шаблона
class graph
{
	mat_t M;
public:
	graph()
	{
		mat_t N(1);
		N[0][0] = 0;
		M = N;
	}

	graph(istream& file)
	{
		mat_t ans;
		string line;
		while (getline(file, line))
		{
			istringstream l_str(line);
			double x;
			vector<double> v;
			while (l_str >> x)
				v.push_back(x);
			assert(!v.empty());
			ans.push_back(v);
			assert(ans[0].size() == v.size());
		}
		M = ans;
	}
	vector<double> way_a(const mat_t&N, int a)
	{
		int n = N.size();
		vector<int> b(n);
		vector<int> b1(n);
		vector<double> d(n);
		d[a] = 0;
		for (int i = 0; i < n; i++)
		{
			b1[i] = 1;
			b[i] = 0;
			if (i != a) d[i] = DBL_MAX;
		}
		for (int i = 0; i<n; i++)
		{
			double k = DBL_MAX;
			int v;
			for (int j = 0; j < n; j++)
			{
				if ((b[j] == 0) && ((d[j] < k)))
				{
					k = d[j];
					v = j;
				}
			}
			b[v] = 1;
			for (int u = 0; u < n; u++)
			{
				if ((M[v][u] != 0) && (b[u] == 0))
				{
					if (d[v] + M[v][u] < d[u]) d[u] = d[v] + M[v][u];
				}
			}
		}
		return d;
	}

	void Way()
	{
		mat_t N = M;
		int n = N.size();
		int x = 0;
		M[x] = way_a(N, x);

	}

	void print_mat()
	{
		for (int i = 0; i < M.size(); i++)
		{
			for (int j = i; j < M.size(); j++)
				cout << M[i][j] << "  ";
			cout << endl;
		}
	}
	int size_m()
	{
		return M.size();
	}
	mat_t matr()
	{
		return M;
	}
};



void GraphWidget::createGraph()
{
	ifstream file_in;
	file_in.open("text.txt");
	graph G(file_in);
	QGraphicsScene *myscene = scene();
	mat_t N = G.matr();
	const int n_graph = N.size();
	QVector< Node*> nodes;
//Создаем список узлов
	for (int i = 0; i < n_graph; ++i)
	{
		nodes.push_back(new Node(this,i));
		myscene->addItem(nodes[i]);
	}
	for (int i = 0; i < n_graph; i++)
	{
		for (int j = i; j < n_graph; j++)
		{
			if (N[i][j]!=0)
				myscene->addItem(new Edge(nodes[i], nodes[j]));
		}
	}
//создаем список ребер
	//myscene->addItem(new Edge(nodes[0], nodes[1]));	// , 10));
	//myscene->addItem(new Edge(nodes[2-1], nodes[3-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[2-1], nodes[5-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[3-1], nodes[6-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[4-1], nodes[1-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[4-1], nodes[5-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[5-1], nodes[6-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[5-1], nodes[8-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[6-1], nodes[9-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[7-1], nodes[4-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[8-1], nodes[7-1]));	// , 10));
	//myscene->addItem(new Edge(nodes[9-1], nodes[8-1]));	// , 10));
								
//определяем позицию на экране
	for (int i=0; i<n_graph; i++)
		nodes[i]->setPos(i*10, i%4);

/*
	nodes[0]->setPos(-75, -50);
	nodes[1]->setPos(0, -50);
	nodes[2]->setPos(50, -50);
	nodes[3]->setPos(-50, 0);
	nodes[4]->setPos(0, 0);
	nodes[5]->setPos(50, 0);
	nodes[6]->setPos(-50, 50);
	nodes[7]->setPos(0, 50);
	nodes[8]->setPos(50, 50);*/

}

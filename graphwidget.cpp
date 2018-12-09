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
//
//graph - работа с графами
//За основу взят пример из стандартного набора обучающих примеров QT - elasticnodes:
//https://doc.qt.io/qt-5/qtwidgets-graphicsview-elasticnodes-example.html
//
#include "pch.h"
#include "graphwidget.h"
#include "edge.h"
#include "node.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>
#include<fstream>
#include <assert.h>

#include <QKeyEvent>
#include <QRandomGenerator>
using namespace std;
//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
	QGraphicsScene * myscene = new QGraphicsScene(this);
	myscene->setItemIndexMethod(QGraphicsScene::NoIndex);
	myscene->setSceneRect(-400, -400, 800, 800);
    setScene(myscene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(800, 800);
    setWindowTitle(tr("Edge + Nodes"));

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

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
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
            item->setPos(-300 + QRandomGenerator::global()->bounded(600), -2*150 + QRandomGenerator::global()->bounded(600));
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

	vector<double> Way()
	{
		mat_t N = M;
		int n = N.size();
		int x = 1;
		M[x] = way_a(N, x);
		return M[x];
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
	mat_t N = G.matr();
	vector<double> N1=G.Way();
	
	QGraphicsScene *myscene = scene();
	const int n_graph = N.size();
	QVector< Node*> nodes;

	for (int i = 0; i < n_graph; ++i)
	{
		nodes.push_back(new Node(this,i, N1[i], 40));
		myscene->addItem(nodes[i]);
	}
	for (int i = 0; i < n_graph; i++)
	{
		for (int j = i; j < n_graph; j++)
		{
			if (N[i][j]!=0)
				myscene->addItem(new Edge(nodes[i], nodes[j], N[i][j]));
		}
	}
	double a = 2*M_PI / n_graph;
	for (int i = 0; i < n_graph; i++)
	{
		nodes[i]->setPos(200 * cos(i*a), 200 * sin(i*a));
		assert(i == nodes[i]->number);
	}
	

	file_in.close();
}

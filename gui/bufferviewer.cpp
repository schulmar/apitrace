#include "bufferviewer.h"
#include <cstdio>

class ArrayModel : public QAbstractItemModel {
public:
  ArrayModel(QByteArray const &data, GLSLStruct const &glslStruct)
      : glslStruct{glslStruct}, data_{data} {
    for (int i = 0; i < static_cast<int>(glslStruct.members.size()); ++i) {
      setHeaderData(i, Qt::Horizontal, glslStruct.members[i]->name);
    }
  }

  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override {
    QModelIndex result;
    if (!parent.isValid()) {
      result = createIndex(row, column);
    }
    return result;
  }

  QModelIndex parent(const QModelIndex &) const override {
    // only have children of the invisible root
    return QModelIndex();
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) {
      return 0;
    } else {
      return data_.size() / glslStruct.sizeInArray;
    }
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) {
      return 0;
    } else {
      return glslStruct.members.size();
    }
  }

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override {
    QVariant result;
    if (index.isValid() && !index.parent().isValid()) {
      if (role == Qt::DisplayRole) {
        if (index.column() < static_cast<int>(glslStruct.members.size())) {
          result = glslStruct.members[index.column()]->interpret(data_);
        }
      }
    }
    return result;
  }

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override {
    if (orientation == Qt::Horizontal) {
    	switch(role) {
    	case Qt::DisplayRole:
    		return glslStruct.members[section]->name;
        case Qt::ToolTipRole:
          return QString("offset=%0").arg(glslStruct.members[section]->offset);
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
  }

private:
  GLSLStruct const &glslStruct;
  QByteArray const &data_;
};

BufferViewer::BufferViewer(QWidget *parent) : QWidget{parent} { setupUi(this); }

void BufferViewer::setDataLayout(GLSLStruct &&glslStruct) {
  this->glslStruct = std::move(glslStruct);
  auto model = new ArrayModel(data, this->glslStruct);
  headerData->setModel(model);
}

void BufferViewer::setBufferData(QByteArray data) { this->data = data; }

#pragma once

#include "ui_bufferviewer.h"
#include <QWidget>

#include <memory>
#include <vector>
#include <cstring>

struct GLSLStruct {
  struct MemberBase {
    QString name;
    int offset;

    MemberBase(QString name, int offset) : name{name}, offset{offset} {}

    virtual QVariant interpret(QByteArray const &data) const = 0;
  };

  template <class Type> struct Member : MemberBase {
    using MemberBase::MemberBase;
    using type = Type;
    QVariant interpret(QByteArray const &data) const override {
      Type value;
      std::memcpy(&value, data.data() + offset, sizeof(value));
      return QVariant::fromValue(value);
    }
  };
  std::vector<std::unique_ptr<MemberBase>> members;
  int sizeInArray;
};

class BufferViewer : public QWidget, Ui_BufferViewer {
public:
  BufferViewer(QWidget *parent = nullptr);

  void setDataLayout(GLSLStruct &&);
  void setBufferData(QByteArray data);
private:
  GLSLStruct glslStruct;
  QByteArray data;
};

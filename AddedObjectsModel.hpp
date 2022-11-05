#ifndef ADDEDOBJECTSMODEL_HPP
#define ADDEDOBJECTSMODEL_HPP

#include <QAbstractListModel>
#include <QReadWriteLock>
#include "FigureData.hpp"
#include <memory>
#include <vector>

class AddedObjectsModel : public QAbstractListModel
{
  Q_OBJECT
public:
  enum FigureRoles {
    TypeRole = Qt::UserRole + 1,
    TypeStrRole,
    CountRole,
    ScaleXRole,
    ScaleYRole,
    ScaleZRole
  };

  AddedObjectsModel();
  AddedObjectsModel(std::vector<std::unique_ptr<FigureData>> &&figures);

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QHash<int, QByteArray> roleNames() const override;

public slots:
  void addObject(int type, int count, qreal scaleX, qreal scaleY, qreal scaleZ);
  void removeObject(int index);

private:
  std::vector<std::unique_ptr<FigureData>> figures;
  std::unique_ptr<QReadWriteLock> lockFigures;
};

#endif // ADDEDOBJECTSMODEL_HPP

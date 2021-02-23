#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QAbstractListModel>
#include "SARibbonBar.h"
//SARibbonActionsManager 特有
class SARibbonActionsManagerPrivate;
//SARibbonActionsModel 特有
class SARibbonActionsModelPrivete;
//SARibbonBarModel 特有
class SARibbonBarModelPrivate;
//SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonCustomizeWidgetPrivate;
class SARibbonMainWindow;
class QStandardItemModel;
class QStandardItem;

/**
 * @brief 用于管理SARibbon的所有Action，
 */
class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
    Q_OBJECT
    friend class SARibbonActionsManagerModel;
public:

    /**
     * @brief 定义action的标签
     */
    enum ActionTag {
        InvalidActionTag		= 0             ///< 无效的tag
        , CommonlyUsedActionTag		= 0x01          ///< 常用命令
        , NotInFunctionalAreaActionTag	= 0x02          ///< 不在功能区命令
        , UserDefineActionTag		= 0x8000        ///< 自定义标签
    };
    typedef QMap<int, QList<QAction *> >::iterator ActionRef;
    SARibbonActionsManager(QObject *p = nullptr);
    ~SARibbonActionsManager();
    //设置tag对应的名字
    void setTagName(int tag, const QString& name);

    //获取tag对应的名字
    QString tagName(int tag) const;

    //注册action
    bool registeAction(QAction *act, int tag, const QString& key = QString());

    //取消action的注册
    void unregisteAction(QAction *act, bool enableEmit = true);

    //过滤得到actions对应的引用，实际是一个迭代器
    ActionRef filter(int tag);

    //判断返回的ActionRefs是否有效，如果没有tag，返回的ActionRefs是无效的
    bool isActionRefsValid(ActionRef r) const;

    //直接得到一个无效的ActionRefs
    ActionRef invalidActionRefs() const;

    //获取所有的标签
    QList<int> actionTags() const;

    //通过key获取action
    QAction *action(const QString& key) const;

    //返回所有管理的action数
    int count() const;

signals:

    /**
     * @brief 标签变化触发的信号，变化包括新增和删除
     */
    void actionTagChanged(int tag, bool isdelete);

private slots:
    void onActionDestroyed(QObject *o);

private:
    void removeAction(QAction *act, bool enableEmit = true);

private:
    SARibbonActionsManagerPrivate *m_d;
};

/**
 * @brief SARibbonActionsManager 对应的model
 */
class SA_RIBBON_EXPORT SARibbonActionsManagerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SARibbonActionsManagerModel(QObject *p = nullptr);
    explicit SARibbonActionsManagerModel(SARibbonActionsManager *m, QObject *p = nullptr);
    ~SARibbonActionsManagerModel();
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    void setFilter(int tag);
    void update();
    void setupActionsManager(SARibbonActionsManager *m);
    void uninstallActionsManager();
    QAction *indexToAction(QModelIndex index) const;

private slots:
    void onActionTagChanged(int tag, bool isdelete);

private:
    SARibbonActionsModelPrivete *m_d;
};

/**
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于@ref SARibbonActionsManager 要在SARibbonActionsManager之后使用此类
 */
class SA_RIBBON_EXPORT SARibbonCustomizeData {
public:
    SARibbonCustomizeData();
private:
    int m_level;            ///< 标记这个data是category还是pannel亦或是action
    int m_categoryIndex;    ///< 在level为pannel和action时有效
    int m_pannelIndex;      ///< 在level为action时有效
    QString m_key;          ///< 在level为category时，key为category标题，level为pannel时key为pannel标题，level为aciton时，key为action的查询依据，基于SARibbonActionsManager查询
    SARibbonActionsManager *m_mgr;
};

/**
 * @brief 自定义界面窗口
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonCustomizeWidget(SARibbonMainWindow *ribbonWindow, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~SARibbonCustomizeWidget();

    /**
     * @brief 定义ribbon树的显示类型
     */
    enum RibbonTreeShowType {
        ShowAllCategory         ///< 显示所有Category，包括contextcategory
        , ShowMainCategory      ///< 显示主要的category，不包含上下文
    };

    /**
     * @brief QStandardItem对应的role
     */
    enum ItemRole {
        LevelRole	= Qt::UserRole + 1      ///< 代表这是层级，有0：category 1：pannel 2：item
        , PointerRole	= Qt::UserRole + 2      ///< 代表这是存放指针。根据LevelRole来进行转
        , CustomizeRole = Qt::UserRole + 3      ///< 代表这个是自定义的item
    };

    /**
     * @brief 定义自定义的动作 对应CustomizeRole的值
     */
    enum CustomizeAction {
        Unknow			= 0     ///< 未知
        , AddCustomizeAction	= 0x01  ///< 添加
        , RemoveCustomizeAction = 0x02  ///< 删除
        , RenameCustomizeAction = 0x04  ///< 重命名
    };

    //设置action管理器
    void setupActionsManager(SARibbonActionsManager *mgr);

    //判断用户是否有改动内容
    bool isChanged() const;

    //获取model
    const QStandardItemModel *model() const;

    //根据当前的radiobutton选项来更新model
    void updateModel();

    //更新model
    void updateModel(RibbonTreeShowType type);

protected:
    QAction *selectedAction() const;

    QStandardItem *selectedItem() const;


    //获取选中的ribbon tree 的level
    int selectedRibbonLevel() const;

    //根据选中的item判断
    int selectedRibbonLevel(QStandardItem *item) const;

    //设置某个item被选中
    void setSelectItem(QStandardItem *item, bool ensureVisible = true);

    //判断itemn能否改动，可以改动返回true
    bool isItemCanCustomize(QStandardItem *item) const;
    bool isSelectedItemCanCustomize() const;

    //删除一个item
    void removeItem(QStandardItem *item);

private slots:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton *b);
    void onPushButtonNewCategoryClicked();
    void onPushButtonNewPannelClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex& index);
    void onTreeViewResultClicked(const QModelIndex& index);

private:
    void initConnection();

private:
    SARibbonCustomizeWidgetUi *ui;
    SARibbonCustomizeWidgetPrivate *m_d;
};



#endif // SARIBBONCUSTOMIZEWIDGET_H
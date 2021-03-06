
//此源码被清华学神尹成大魔王专业翻译分析并修改
//尹成QQ77025077
//尹成微信18510341407
//尹成所在QQ群721929980
//尹成邮箱 yinc13@mails.tsinghua.edu.cn
//尹成毕业于清华大学,微软区块链领域全球最有价值专家
//https://mvp.microsoft.com/zh-cn/PublicProfile/4033620
//版权所有（c）2011至今，Facebook，Inc.保留所有权利。
//此源代码在两个gplv2下都获得了许可（在
//复制根目录中的文件）和Apache2.0许可证
//（在根目录的license.apache文件中找到）。

#pragma once
#ifndef ROCKSDB_LITE

#include "rocksdb/utilities/transaction_db_mutex.h"

namespace rocksdb {

class TransactionDBMutex;
class TransactionDBCondVar;

//TransactionDBmutexFactory的默认实现。可能被重写
//通过transactiondboptions.custom_mutex_工厂。
class TransactionDBMutexFactoryImpl : public TransactionDBMutexFactory {
 public:
  std::shared_ptr<TransactionDBMutex> AllocateMutex() override;
  std::shared_ptr<TransactionDBCondVar> AllocateCondVar() override;
};

}  //命名空间rocksdb

#endif  //摇滚乐

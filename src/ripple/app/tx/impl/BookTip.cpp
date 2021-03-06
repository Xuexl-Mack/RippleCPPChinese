
//此源码被清华学神尹成大魔王专业翻译分析并修改
//尹成QQ77025077
//尹成微信18510341407
//尹成所在QQ群721929980
//尹成邮箱 yinc13@mails.tsinghua.edu.cn
//尹成毕业于清华大学,微软区块链领域全球最有价值专家
//https://mvp.microsoft.com/zh-cn/PublicProfile/4033620
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————
/*
    此文件是Rippled的一部分：https://github.com/ripple/rippled
    版权所有（c）2014 Ripple Labs Inc.

    使用、复制、修改和/或分发本软件的权限
    特此授予免费或不收费的目的，前提是
    版权声明和本许可声明出现在所有副本中。

    本软件按“原样”提供，作者不作任何保证。
    关于本软件，包括
    适销性和适用性。在任何情况下，作者都不对
    任何特殊、直接、间接或后果性损害或任何损害
    因使用、数据或利润损失而导致的任何情况，无论是在
    合同行为、疏忽或其他侵权行为
    或与本软件的使用或性能有关。
**/

//==============================================================

#include <ripple/app/tx/impl/BookTip.h>
#include <ripple/basics/Log.h>

namespace ripple {

BookTip::BookTip (ApplyView& view, Book const& book)
    : view_ (view)
    , m_valid (false)
    , m_book (getBookBase (book))
    , m_end (getQualityNext (m_book))
{
}

bool
BookTip::step (beast::Journal j)
{
    if (m_valid)
    {
        if (m_entry)
        {
            offerDelete (view_, m_entry, j);
            m_entry = nullptr;
        }
    }

    for(;;)
    {
//看看有没有比当前质量更差的条目。通知
//质量仅编码在第一页的索引中
//目录的
        auto const first_page =
            view_.succ (m_book, m_end);

        if (! first_page)
            return false;

        unsigned int di = 0;
        std::shared_ptr<SLE> dir;

        if (dirFirst (view_, *first_page, dir, di, m_index, j))
        {
            m_dir = dir->key();
            m_entry = view_.peek(keylet::offer(m_index));
            m_quality = Quality (getQuality (*first_page));
            m_valid = true;

//下一个查询应在此目录之前开始
            m_book = *first_page;

//质量在下一个质量之前
            --m_book;

            break;
        }

//不应该有空目录，但以防万一，
//我们通过前进到下一个目录来处理这个案例。
        m_book = *first_page;
    }

    return true;
}

}

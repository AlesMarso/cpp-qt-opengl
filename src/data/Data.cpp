#include "Data.h"

namespace data
{

	CDataItem::CDataItem()
		: m_dataItem()
		, m_dataType(TDataItemType::Empty)
	{
	}

	CDataItem::CDataItem(const TValue& value)
	{
		m_dataType = TDataItemType::Value;
		m_dataItem = value;
	}

	CDataItem::CDataItem(const TCandle& candle)
	{
		m_dataType = TDataItemType::Candle;
		m_dataItem = candle;
	}

	CDataItem::~CDataItem()
	{
	}

	TDataItemType CDataItem::GetDataType() const
	{
		return m_dataType;
	}

	const TValue& CDataItem::AsValue() const
	{
		if (m_dataType != TDataItemType::Value)
			throw std::bad_variant_access();

		return std::get<TValue>(m_dataItem);
	}

	const TCandle& CDataItem::AsCandle() const
	{
		if (m_dataType != TDataItemType::Candle)
			throw std::bad_variant_access();

		return std::get<TCandle>(m_dataItem);
	}

	TValue& CDataItem::AsValue()
	{
		if (m_dataType != TDataItemType::Value)
			throw std::bad_variant_access();

		return std::get<TValue>(m_dataItem);
	}

	TCandle& CDataItem::AsCandle()
	{
		if (m_dataType != TDataItemType::Candle)
			throw std::bad_variant_access();

		return std::get<TCandle>(m_dataItem);
	}

	void CDataItem::Reset()
	{
		m_dataType = TDataItemType::Empty;
		m_dataItem = std::monostate{};
	}

	bool CDataItem::IsEmpty() const
	{
		return m_dataType == TDataItemType::Empty;
	}

	CDataSource::CDataSource(TDataItemType type, int64_t id)
		: CData(type, id)
	{

	}

	CDataSource::~CDataSource()
	{

	}

	std::shared_ptr<CDataChild> CreateChildData(const TDataType type, const int64_t id, std::weak_ptr<CDataSource> parent)
	{
		switch (type)
		{
		case TDataType::Price:
			return std::make_shared<CDataPrice>(id, parent);

		default:
			return nullptr;
		}
	}

	std::shared_ptr<CDataChild> CDataSource::CreateChild(TDataType type)
	{
		int64_t id = static_cast<int64_t>(m_childs.size()) + 1;
		auto child = CreateChildData(type, id, weak_from_this());
		m_childs.push_back(child);
		return child;
	}

	void CDataSource::RemoveChild(const std::shared_ptr<CDataChild>& child)
	{
		auto it = std::find(m_childs.begin(), m_childs.end(), child);

		if (it == m_childs.end())
			return;

		RemoveChild(it);
	}

	int64_t CDataSource::AddDataItem(const CDataItem& item)
	{
		m_allData.push_back(item);
		int64_t id = static_cast<int64_t>(m_allData.size() - 1);
		for (auto& child : m_childs)
		{
			if (child)
			{
				child->AddDataItemToParent(id);
			}
		}
		return id;
	}

	void CDataSource::RemoveChild(std::list<std::shared_ptr<CDataChild>>::iterator it)
	{
		m_childs.erase(it);
	}

	CDataChild::CDataChild(TDataType type, int64_t id, std::weak_ptr<CDataSource> parent)
		: CData(TDataItemType::Candle, id)
		, m_parent(parent)
		, m_type(type)
	{

	}

	CDataChild::~CDataChild()
	{
	}

	void CDataChild::SetParent(std::weak_ptr<CDataSource> parent)
	{
		m_parent = parent;
	}

	int64_t CDataChild::AddDataItemToParent(int64_t id)
	{
		if (auto pParent = m_parent.lock())
		{
			if (in_range(id, pParent->size()))
			{
				m_allData.push_back(pParent->GetItem(id));
				return static_cast<int64_t>(m_allData.size() - 1);
			}
		}

		return -1;
	}

	void CDataChild::Calculate(const int64_t from, const int64_t to)
	{
		if (auto pParent = m_parent.lock())
		{
			for (auto id = from; id < to; ++id)
			{
				if (in_range(id, pParent->size()))
				{
					auto& parentDataItem = pParent->GetItem(id);

				}
			}
		}
	}

	CDataPrice::CDataPrice(int64_t id, std::weak_ptr<CDataSource> parent)
		: CDataChild(TDataType::Price, id, parent)
	{
	}

	CDataPrice::~CDataPrice()
	{
	}
	void CDataPrice::Calculate(const int64_t from, const int64_t to)
	{
	}
}

#pragma once

#include <variant>
#include <vector>
#include <list>
#include <memory>
#include <optional>

namespace data
{
	enum class TDataItemType : int16_t
	{
		Unknown,
		Empty,
		Candle,
		Value,
	};

	enum class TDataType : int32_t
	{
		Unknow,
		Price,
		Volume
	};

	struct TCandle
	{
		double m_open;
		double m_high;
		double m_low;
		double m_close;

		TCandle(double open, double high, double low, double close)
			: m_open(open)
			, m_high(high)
			, m_low(low)
			, m_close(close)
		{
		}

		~TCandle() = default;
	};

	using TValue = float;

	class CDataItem
	{
	public:
		CDataItem();
		CDataItem(const TValue& value);
		CDataItem(const TCandle& candle);
		~CDataItem();

		TDataItemType GetDataType() const;

		const TValue& AsValue() const;
		const TCandle& AsCandle() const;

		TValue& AsValue();
		TCandle& AsCandle();

		void Reset();

		bool IsEmpty() const;

	private:
		std::variant<std::monostate, TCandle, TValue> m_dataItem{ std::monostate{} };
		TDataItemType m_dataType{ TDataItemType::Empty };
	};

	class CData
	{
	public:
		CData(TDataItemType type, int64_t id)
			: m_dataType(type)
			, m_id(id)
		{

		}

		virtual ~CData()
		{

		}

		int64_t GetID() const
		{
			return m_id;
		}

		void SetID(int64_t id)
		{
			m_id = id;
		}

		const CDataItem& GetItem(int64_t id) const
		{
			if (in_range(id, m_allData))
				return m_allData[id];

			throw std::exception("");
		}

		CDataItem& GetItem(int64_t id)
		{
			if (in_range(id, m_allData))
				return m_allData[id];

			throw std::exception("");
		}

		size_t size() const
		{
			return m_allData.size();
		}

	protected:
		std::vector<CDataItem> m_allData;
		TDataItemType m_dataType{ TDataItemType::Empty };
		int64_t m_id{ -1 };
	};

	class CDataSource;
	class CDataChild;

	class CDataSource : public CData, public std::enable_shared_from_this<CDataSource>
	{
	public:
		CDataSource(TDataItemType type, int64_t id);
		~CDataSource();

		std::shared_ptr<CDataChild> CreateChild(TDataType type);

		void RemoveChild(const std::shared_ptr<CDataChild>& child);

		int64_t AddDataItem(const CDataItem& item);

	protected:
		void RemoveChild(std::list<std::shared_ptr<CDataChild>>::iterator it);

	protected:
		std::list<std::shared_ptr<CDataChild>> m_childs;
	};

	class CDataChild : public CData
	{
	public:
		CDataChild(TDataType type, int64_t id, std::weak_ptr<CDataSource> parent);
		~CDataChild();

		void SetParent(std::weak_ptr<CDataSource> parent);

		int64_t AddDataItemToParent(int64_t id);

		virtual void Calculate(const int64_t from, const int64_t to);

	protected:
		std::weak_ptr<CDataSource> m_parent;
		TDataType m_type{ TDataType::Unknow };
	};

	class CDataPrice : public CDataChild
	{
	public:
		CDataPrice(int64_t id, std::weak_ptr<CDataSource> parent);
		~CDataPrice();

		void Calculate(const int64_t from, const int64_t to) override;
	};
}
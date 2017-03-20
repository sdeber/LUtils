
#include <boost/random.hpp>

#include "../src/details/TAMTNode.h"
#include "../src/TPrefixTree.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/chrono.hpp>
#include <set>
#include <unordered_set>

const std::vector<std::string>  knownmccmnc_list{
"41201",
"41220",
"41230",
"41240",
"41288",
"27601",
"27602",
"27603",
"27604",
"60301",
"60302",
"21303",
"63102",
"365010",
"344030",
"344920",
"344930",
"722010",
"722020",
"722070",
"722310",
"722320",
"722330",
"722341",
"36301",
"50501",
"50502",
"50503",
"50504",
"50505",
"50506",
"50507",
"50508",
"50509",
"50510",
"50511",
"50512",
"50513",
"50514",
"50515",
"50516",
"50517",
"50518",
"50519",
"50524",
"50571",
"50572",
"50588",
"50590",
"50599",
"23201",
"23202",
"23203",
"23204",
"23205",
"23206",
"23207",
"23209",
"23210",
"23211",
"23212",
"23214",
"23215",
"23291",
"40001",
"40002",
"40003",
"40004",
"42601",
"47001",
"47002",
"47003",
"342600",
"342820",
"25701",
"25702",
"20601",
"20610",
"20620",
"70267",
"70268",
"61601",
"61602",
"61603",
"40211",
"40217",
"73601",
"73602",
"73603",
"21803",
"21805",
"21890",
"65201",
"65202",
"65204",
"72400",
"72401",
"72402",
"72403",
"72404",
"72405",
"72406",
"72407",
"72408",
"72409",
"72410",
"72411",
"72412",
"72413",
"72414",
"72415",
"72416",
"72417",
"72418",
"72419",
"72421",
"72423",
"72425",
"72427",
"72429",
"72431",
"72433",
"72435",
"72437",
"72439",
"72441",
"72443",
"72445",
"72447",
"72448",
"72451",
"72453",
"72455",
"72457",
"72459",
"348170",
"348370",
"348570",
"348770",
"52811",
"28401",
"28405",
"61302",
"61303",
"64201",
"64202",
"64203",
"64207",
"64208",
"64282",
"45601",
"45602",
"45603",
"45604",
"45605",
"45606",
"45608",
"45618",
"62401",
"62402",
"30236",
"30237",
"30262",
"30263",
"30264",
"302656",
"30266",
"30267",
"30268",
"30271",
"30272",
"30286",
"62501",
"62502",
"346140",
"62301",
"62302",
"62303",
"62201",
"62202",
"73001",
"73002",
"73003",
"73004",
"73005",
"73006",
"73007",
"73010",
"46000",
"46001",
"46003",
"46004",
"732001",
"732002",
"732020",
"732099",
"732101",
"732102",
"732103",
"732111",
"732123",
"732130",
"65401",
"62901",
"62910",
"54801",
"71201",
"61202",
"61203",
"61204",
"61205",
"61206",
"61207",
"21901",
"21902",
"21910",
"36801",
"28001",
"28010",
"23001",
"23002",
"23003",
"23004",
"23098",
"23099",
"63001",
"63005",
"63086",
"23801",
"23802",
"23803",
"23806",
"23807",
"23810",
"23812",
"23820",
"23830",
"23877",
"63801",
"37001",
"37002",
"37003",
"37004",
"74000",
"74001",
"74002",
"60201",
"60202",
"60203",
"70601",
"70602",
"70603",
"62701",
"24801",
"24802",
"24803",
"24804",
"24805",
"24806",
"24807",
"24871",
"63601",
"750001",
//"27402",
"28801",
"28802",
"54201",
//"54202",
"54203",
"24404",
"24405",
"24409",
"24412",
"24414",
"24416",
"24421",
"24491",
"20801",
"20802",
"20805",
"20806",
"20807",
"20810",
"20811",
"20813",
"20820",
"20821",
"20822",
"20888",
"64700",
"64702",
"64710",
"34011",
"54702",
"54710",
"54720",
"62801",
"62802",
"62803",
"62804",
"60701",
"60702",
"60703",
"28201",
"28202",
"28203",
"28204",
"28205",
"26201",
"26202",
"26203",
"26204",
"26205",
"26206",
"26207",
"26208",
"26209",
"26210",
"26211",
"26212",
"26213",
"26214",
"26215",
"26276",
"26277",
"62001",
"62002",
"62003",
"62004",
"62011",
"26601",
"26606",
"26609",
"20201",
"20205",
"20209",
"20210",
"29001",
"34001",
"34002",
"34003",
"34008",
"34010",
"34020",
"70401",
"70402",
"70403",
"61101",
"61102",
"61105",
"63201",
"63202",
"73801",
"37201",
"37202",
"37203",
"708001",
"708002",
"708040",
"45400",
"45401",
"45402",
"45403",
"45404",
"45405",
"45406",
"45407",
"45408",
"45409",
"45410",
"45411",
"45412",
"45415",
"45416",
"45418",
"45419",
"21601",
"21630",
"21670",
"27401",
"27402",
"27403",
"27404",
"27407",
"40400",
"40401",
"40402",
"40403",
"40404",
"40405",
"40406",
"40407",
"40409",
"40410",
"40411",
"40412",
"40413",
"40414",
"40415",
"40416",
"40417",
"40418",
"40419",
"40420",
"40421",
"40422",
"40423",
"40424",
"40425",
"40427",
"40429",
"40430",
"40431",
"40433",
"40434",
"40435",
"40436",
"40437",
"40438",
"40440",
"40441",
"40442",
"40443",
"40444",
"40446",
"40448",
"40449",
"40450",
"40451",
"40452",
"40453",
"40454",
"40455",
"40456",
"40457",
"40458",
"40459",
"40460",
"40461",
"40462",
"40463",
"40464",
"40465",
"40466",
"40467",
"40468",
"40469",
"40470",
"40471",
"40472",
"40473",
"40474",
"40475",
"40476",
"40477",
"40478",
"40479",
"40480",
"40481",
"40482",
"40483",
"40484",
"40485",
"40486",
"40487",
"40488",
"40489",
"40490",
"40491",
"40492",
"40493",
"40494",
"40495",
"40496",
"40497",
"40498",
"40499",
"40500",
"40501",
"40503",
"40504",
"40505",
"40506",
"40507",
"40508",
"40509",
"40510",
"40511",
"40512",
"40513",
"40514",
"40515",
"40517",
"40518",
"40520",
"40521",
"40522",
"40523",
"40525",
"40527",
"40528",
"40529",
"40530",
"40531",
"40532",
"40534",
"40535",
"40536",
"40537",
"40538",
"40539",
"40541",
"40542",
"40543",
"40544",
"40545",
"40546",
"40547",
"40552",
"40553",
"40554",
"40555",
"40556",
"40566",
"40567",
"40568",
"40570",
"40571",
"40572",
"40573",
"40574",
"40575",
"40576",
"40577",
"40580",
"40581",
"40582",
"40583",
"40584",
"40585",
"40586",
"51000",
"51001",
"51008",
"51010",
"51011",
"51021",
"51028",
"90101",
"90102",
"90103",
"90105",
"90106",
"90109",
"90110",
"90111",
"90112",
"90113",
"90114",
"90115",
"90116",
"90117",
"90118",
"90119",
"90120",
"90121",
"90122",
"90123",
"90124",
"90125",
"90126",
"90127",
"90128",
"90129",
"90130",
"90188",
"43211",
"43214",
"43219",
"41805",
"41820",
"41830",
"41840",
"41847",
"41848",
"41849",
"41862",
"41870",
"41880",
"41881",
"41883",
"41884",
"41885",
"41886",
"41887",
"41888",
"41889",
"41891",
"41892",
"41893",
"41894",
"27201",
"27202",
"27203",
"27207",
"27209",
"42501",
"42502",
"42503",
"42504",
"42506",
"22201",
"22202",
"22210",
"22277",
"22288",
"22298",
"22299",
"338020",
"338050",
"44001",
"44002",
"44003",
"44004",
"44006",
"44007",
"44008",
"44009",
"44010",
"44011",
"44012",
"44013",
"44014",
"44015",
"44016",
"44017",
"44018",
"44019",
"44020",
"44021",
"44022",
"44023",
"44024",
"44025",
"44026",
"44027",
"44028",
"44029",
"44030",
"44031",
"44032",
"44033",
"44034",
"44035",
"44036",
"44037",
"44038",
"44039",
"44040",
"44041",
"44042",
"44043",
"44044",
"44045",
"44046",
"44047",
"44048",
"44049",
"44050",
"44051",
"44052",
"44053",
"44054",
"44055",
"44056",
"44058",
"44060",
"44061",
"44062",
"44063",
"44064",
"44065",
"44066",
"44067",
"44068",
"44069",
"44070",
"44071",
"44072",
"44073",
"44074",
"44075",
"44076",
"44077",
"44078",
"44079",
"44080",
"44081",
"44082",
"44083",
"44084",
"44085",
"44086",
"44087",
"44088",
"44089",
"44090",
"44092",
"44093",
"44094",
"44095",
"44096",
"44097",
"44098",
"44099",
"44140",
"44141",
"44142",
"44143",
"44144",
"44145",
"44150",
"44151",
"44161",
"44162",
"44163",
"44164",
"44165",
"44170",
"44190",
"44191",
"44192",
"44193",
"44194",
"44198",
"44199",
"41601",
"41602",
"41603",
"41677",
"40101",
"40102",
"63902",
"63903",
"45002",
"45003",
"41902",
"41903",
"41904",
"43701",
"45701",
"45702",
"45708",
"24701",
"24702",
"24703",
"24704",
"24705",
"24706",
"24707",
"24708",
"41505",
"41532",
"41533",
"41534",
"41535",
"41536",
"41537",
"41538",
"41539",
"65101",
"65102",
"61804",
"29501",
"29502",
"29505",
"29577",
"24601",
"24602",
"24603",
"27001",
"27077",
"27099",
"45500",
"45501",
"45503",
"64601",
"64602",
"64603",
"64604",
"65001",
"65010",
"50210",
"50212",
"50213",
"50214",
"50216",
"50217",
"50218",
"50219",
"50220",
"47201",
"61001",
"27801",
"27821",
"27877",
"34012",
"60901",
"60902",
"60910",
"61701",
"61702",
"61710",
"334020",
"55001",
"25901",
"25902",
"25904",
"42899",
"29703",
"60400",
"60401",
"64301",
"64304",
"41401",
"64901",
"64903",
"54202",
"42901",
"20402",
"20403",
"20404",
"20405",
"20406",
"20407",
"20408",
"20410",
"20412",
"20414",
"20416",
"20418",
"20420",
"20421",
"20460",
"20469",
"36251",
"36269",
"36291",
"54601",
"53000",
"53001",
"53002",
"53003",
"53004",
"53005",
"53024",
"71021",
"71073",
"61401",
"61402",
"61403",
"62120",
"62130",
"62140",
"62150",
"62160",
"24201",
"24202",
"24203",
"24204",
"42202",
"42203",
"42204",
"41001",
"41003",
"41004",
"41006",
"41007",
"55201",
"71401",
"71402",
"714020",
"71403",
"71404",
"53701",
"53702",
"53703",
"74401",
"74402",
"74403",
"71610",
"51501",
"51502",
"51503",
"51505",
"26001",
"26002",
"26003",
"26004",
"26005",
"26006",
"26007",
"26008",
"26009",
"26010",
"26801",
"26803",
"26805",
"26806",
"42701",
"22601",
"22602",
"22603",
"22610",
"25001",
"25002",
"25003",
"25004",
"25005",
"25007",
"25010",
"25011",
"25012",
"25013",
"25016",
"25017",
"25019",
"25020",
"25028",
"25039",
"25044",
"25092",
"25093",
"25099",
"63510",
"30801",
"54901",
"54927",
"29201",
"62601",
"42001",
"42003",
"60801",
"60802",
"22001",
"22003",
"22005",
"63301",
"63302",
"63310",
"61901",
"61902",
"61903",
"61904",
"61905",
"61925",
"61940",
"61950",
"52501",
"52502",
"52503",
"52505",
"52512",
"23101",
"23102",
"23104",
"23105",
"29340",
"29341",
"29364",
"29370",
"63730",
"65501",
"65502",
"65506",
"65507",
"65510",
"65511",
"65512",
"65513",
"65521",
"65530",
"65531",
"65532",
"65533",
"65534",
"65535",
"65536",
"21401",
"21403",
"21404",
"21405",
"21406",
"21407",
"21408",
"21409",
"21415",
"21416",
"21417",
"21418",
"21419",
"21420",
"21421",
"21422",
"21423",
"21424",
"21425",
"21426",
"41302",
"41303",
"63401",
"63402",
"63405",
"63499",
"74602",
"74603",
"74604",
"74605",
"65301",
"65310",
"24001",
"24002",
"24003",
"24004",
"24005",
"24006",
"24007",
"24008",
"24009",
"24010",
"24011",
"24012",
"24013",
"24014",
"24015",
"24016",
"24017",
"24018",
"24019",
"24020",
"24021",
"24022",
"24024",
"22801",
"22802",
"22803",
"22805",
"22806",
"22807",
"22808",
"22812",
"22851",
"41701",
"41702",
"41709",
"43601",
"43602",
"43603",
"43604",
"43605",
"64002",
"64003",
"64004",
"64005",
"52000",
"52001",
"52015",
"29401",
"29402",
"29403",
"61501",
"53901",
"53943",
"53988",
"37412",
"374130",
"374140",
"60502",
"60503",
"28601",
"28602",
"28603",
"28604",
"43801",
"43802",
"376352",
"376360",
"64101",
"64110",
"64111",
"64114",
"64122",
"64166",
"25501",
"25502",
"25503",
"25504",
"25505",
"25506",
"25507",
"25521",
"42402",
"23400",
"23401",
"23402",
"23403",
"23404",
"23405",
"23406",
"23407",
"23408",
"23409",
"23410",
"23411",
"23412",
"23413",
"23414",
"23415",
"23416",
"23417",
"23418",
"23419",
"23420",
"23421",
"23422",
"23423",
"23424",
"23425",
"23426",
"23430",
"23431",
"23432",
"23433",
"23434",
"23450",
"23455",
"23458",
"23476",
"23478",
"23577",
"23591",
"23592",
"23594",
"23595",
"310010",
"310012",
"310013",
"310016",
"310017",
"310020",
"310030",
"310035",
"310040",
"310050",
"310060",
"310070",
"310080",
"310090",
"310100",
"310110",
"310130",
"310140",
"310150",
"310160",
"310170",
"310180",
"310190",
"310200",
"310210",
"310220",
"310230",
"310240",
"310250",
"310260",
"310270",
"310280",
"310290",
"310300",
"310310",
"310320",
"310340",
"310350",
"310360",
"310370",
"310380",
"310390",
"310400",
"310410",
"310420",
"310430",
"310440",
"310450",
"310460",
"310470",
"310480",
"310490",
"310500",
"310520",
"310530",
"310540",
"310550",
"310560",
"310570",
"310580",
"310590",
"310600",
"310610",
"310620",
"310640",
"310650",
"310660",
"310670",
"310680",
"310690",
"310700",
"310710",
"310720",
"310730",
"310740",
"310750",
"310760",
"310770",
"310780",
"310790",
"310800",
"310810",
"310820",
"310830",
"310840",
"310850",
"310860",
"310870",
"310880",
"310890",
"310900",
"310910",
"310930",
"310940",
"310950",
"310960",
"310970",
"310980",
"310990",
"311000",
"311010",
"311020",
"311030",
"311040",
"311050",
"311060",
"311080",
"311090",
"311100",
"311110",
"311120",
"311140",
"311150",
"311170",
"311180",
"311190",
"311200",
"311220",
"311240",
"311250",
"311260",
"311270",
"311271",
"311272",
"311273",
"311274",
"311275",
"311276",
"311277",
"311278",
"311279",
"311280",
"311281",
"311282",
"311283",
"311284",
"311285",
"311286",
"311287",
"311288",
"311289",
"311290",
"311310",
"311320",
"311330",
"311340",
"311350",
"311360",
"311370",
"311380",
"311390",
"311410",
"311420",
"311430",
"311440",
"311450",
"311460",
"311470",
"311480",
"311481",
"311482",
"311483",
"311484",
"311485",
"311486",
"311487",
"311488",
"311489",
"311490",
"311500",
"311530",
"311540",
"311550",
"311560",
"311570",
"311580",
"311590",
"311600",
"311610",
"311620",
"311630",
"311640",
"311650",
"311660",
"311670",
"311680",
"311690",
"311700",
"311710",
"311720",
"316010",
"316011",
"74800",
"74801",
"74803",
"74807",
"74810",
"43401",
"43402",
"43404",
"43405",
"43407",
"54101",
"54105",
"73401",
"73402",
"73403",
"73404",
"73406",
"45201",
"45202",
"45203",
"45204",
"45207",
"42101",
"42102",
"64501",
"64502",
"64503",
"64801",
"64803",
"64804",
};

template<> struct Alphabet<char,std::string,16>
{
    typedef char Character;
    typedef std::string String;
    
    static unsigned int to_index(char c)
    {
	return c - 48;
    }

    static int to_index_array(std::string::const_iterator& it,
			      std::string::const_iterator& end,
			      uint8_t * array,
			      unsigned int arrayLen)
    {
	int i = 0;
	for(; it != end; ++it, ++i)
	{
	    array[i] = to_index(*it);
	}

	return i;
    }
};

typedef struct Alphabet<char,std::string,16> Digits;
typedef TAMTNode<Digits, Bitmap16> AMT16;

void performace_test()
{
    std::string digits = "0123456789";
    TPrefixTree<AMT16, void> digitTrie;
    std::vector<std::string> numbers;
    boost::random::random_device gen;
    std::vector<std::string> nums;
    std::set<std::string> setBased;
    std::unordered_set<std::string> hashBased;
    const unsigned int COUNT = 500000;
    
    for(int j = 0; j < COUNT; ++j)
    {
	std::string number;
	for(int i = 0; i < 10; ++i)
	{
	    boost::random::uniform_int_distribution<> index_dist(0, 9);
	    number.push_back(digits[index_dist(gen)]);
	}
	nums.push_back(number);

    }

    
    boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now(), end;
    
    for(int j = 0; j < COUNT; ++j)
    {
	digitTrie.Insert(nums[j]);
    }
    end = boost::chrono::steady_clock::now();
    auto test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
	setBased.insert(nums[j]);
    }
    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    
    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
	hashBased.insert(nums[j]);
    }
    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    	
	
	
    

    for(int j = 0; j < COUNT; ++j)
    {
	digitTrie.Search(nums[j]);
    }

    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;


    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
    	setBased.find(nums[j]);
    }

    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;


    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
    	hashBased.find(nums[j]);
    }

    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;
}

void correctness_test()
{
    TPrefixTree<AMT16, void> digitTrie; 
   
    std::vector<std::string> numbers = {"123456789", "123456", "1234568", "123457", "123458", "012345", "0123456789"};
    boost::random::random_device gen;
   
    std::cout << "Numbers Generated" << std::endl;
    
    
    for(int i = 0; i < numbers.size(); ++i)
    {
    	digitTrie.Insert(numbers[i]);
    }
    
    std::cout << "Numbers Inserted" << std::endl;
   
    for(int i = 0; i < numbers.size(); ++i)
    {
    	bool result = digitTrie.Search(numbers[i]);
    	if(!result)
    	{
    	    std::cout << "Failed to search the key: " << numbers[i] << ", at position: " << i << std::endl;
    	    break;
    	}
    }

    std::cout << "Starting to test deletion." << std::endl;

    for(int i = 0; i < numbers.size(); ++i)
    {
	if(!digitTrie.Remove(numbers[i]))
	{
	    std::cout << "Error. Failed to delete the key: " << numbers[i] << ", at position: " << i << std::endl;
	    break;
	}

	bool result = digitTrie.Search(numbers[i]);
	if(result)
	{
	    std::cout << "Failed to search the key: " << numbers[i] << ", at position: " << i << std::endl;
	}
    }
 
}


void correctness_test2()
{
    TPrefixTree<AMT16, int> digitTrie; 
   
    
    for(int i = 0; i < knownmccmnc_list.size(); ++i)
    {
    	digitTrie.Insert(knownmccmnc_list[i],1);
    }
    
    std::cout << "Numbers Inserted" << std::endl;
   
    for(int i = 0; i < knownmccmnc_list.size(); ++i)
    {
      int r = 0;
    	bool result = digitTrie.Search(knownmccmnc_list[i],r);
    	if(!result)
    	{
    	    std::cout << "Failed to search the key: " << knownmccmnc_list[i] << ", at position: " << i << std::endl;
    	    break;
    	}
    }

    std::cout << "Starting to test deletion." << std::endl;

    for(int i = 0; i < knownmccmnc_list.size(); ++i)
    {
	if(!digitTrie.Remove(knownmccmnc_list[i]))
	{
	    std::cout << "Error. Failed to delete the key: " << knownmccmnc_list[i] << ", at position: " << i << std::endl;
	    break;
	}

	int r = 0;
	bool result = digitTrie.Search(knownmccmnc_list[i], r);
	if(result)
	{
	    std::cout << "Failed to search the key: " << knownmccmnc_list[i] << ", at position: " << i << std::endl;
	}
    }
 
}

int main(int argc, char** argv)
{
    int num = 0;
    if(argc > 1)
    {
	num = atoi(argv[1]);
    }

    if(num == 0)
	num = 1000000;

    //performace_test();
    correctness_test2();
}

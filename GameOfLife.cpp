#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <set>
#include <utility>
#include <functional>

typedef std::pair<int,int> addr_t;

typedef std::set<addr_t> board_t;

class GameLogic
{
    void walkNeighbors(addr_t cellAddr, std::function<void(addr_t)> fn)
    {
        for( int i = -1; i < 2; i++ )
        {
            for( int j = -1; j < 2; j++ )
            {
                if( j != 0 || i != 0 )
                {
                    addr_t cell{cellAddr.first + i, cellAddr.second + j};
                    fn(cell);
                }
            }
        }
    }

public:
    GameLogic() {}
    virtual int numNeighbors(addr_t cellAddr, board_t world)
    {
        auto num = 0;
        auto fn = [&](addr_t cell){ if( world.find(cell) != world.end() ) num++; };
        walkNeighbors(cellAddr, fn);
        return num;
    }

    virtual board_t getDeadNeighbors(board_t world)
    {
        board_t deadNeighbors;
        auto fn = [&](addr_t cell){if( world.find(cell) == world.end() ) deadNeighbors.insert(cell); };
        for( auto c = world.begin(); c != world.end(); c++ )
        {
            walkNeighbors(*c, fn);
        }
        return deadNeighbors;
    }
};

class MockGameLogic : public GameLogic
{
    int num;
    board_t world;
public:
    MockGameLogic(int num) : num(num) {}
    int setWorld(board_t world)
    {
        this->world = world;
    }
    virtual int numNeighbors(addr_t cellAddr, board_t world)
    {
        return num;
    }
    virtual board_t getDeadNeighbors(board_t world)
    {
        return this->world;
    }
};

class GameOfLife
{
    board_t world;
    GameLogic stdLogic;
    GameLogic* logic;
public:
    GameOfLife():logic(&stdLogic) {}
    GameOfLife(board_t init)
        : world(init),
          logic(&stdLogic)
    {}
    board_t livingMembers()
    {
        return world;
    }
    void step(int times)
    {
        for( auto i = 0; i < times; i++ )
        {
            step();
        }
    }

    void step()
    {
        board_t nextGen;

        for(auto i = world.begin(); i != world.end(); i++ )
        {
            auto numNeighbors = logic->numNeighbors(*i, world);
            if( numNeighbors == 2 )
            {
                nextGen.insert(*i);
            }
        }

        auto dead = logic->getDeadNeighbors(world);
        dead.insert(world.begin(), world.end());

        for(auto i = dead.begin(); i != dead.end(); i++ )
        {
            if( logic->numNeighbors(*i, world) == 3 )
            {
                nextGen.insert(*i);
            }
        }
        world = nextGen;
    }

    // For Mocking purposes only...
    void setGameLogic(GameLogic* logic)
    {
        this->logic = logic;
    }
};

class GameOfLifeBoard : public ::testing::Test
{
};

TEST_F(GameOfLifeBoard, IsEmptyByDefault)
{
    GameOfLife gol;
    EXPECT_THAT(gol.livingMembers(), ::testing::ElementsAre());
}

TEST_F(GameOfLifeBoard, CanBeInitialized)
{
    GameOfLife gol(board_t({{1,2}}));
    board_t expected{{1,2}};
    EXPECT_EQ(gol.livingMembers(), expected);
}

TEST_F(GameOfLifeBoard, CanStep)
{
    board_t board{{1,1}};
    board.insert({1,2});
    board.insert({1,3});
    GameOfLife gol(board);
    gol.step();
    EXPECT_THAT(gol.livingMembers(), ::testing::Not(::testing::ElementsAre()));
}

TEST_F(GameOfLifeBoard, KeepsCellAliveWhenItHas2Neighbors)
{
    GameOfLife gol(board_t{{1,2}, {5,5}, {5,4}, {5,3}});
    MockGameLogic logic(2);
    gol.setGameLogic(&logic);
    gol.step();
    EXPECT_THAT(gol.livingMembers(), ::testing::Contains(addr_t({1,2})));
}

TEST_F(GameOfLifeBoard, KeepsCellAliveWhenItHas3Neighbors)
{
    GameOfLife gol(board_t{{1,2}, {5,5}, {5,4}, {5,3}});
    MockGameLogic logic(3);
    gol.setGameLogic(&logic);
    gol.step();
    EXPECT_THAT(gol.livingMembers(), ::testing::Contains(addr_t({1,2})));
}
TEST_F(GameOfLifeBoard, KillsCellWhenItHas4Neighbors)
{
    GameOfLife gol(board_t{{1,2}, {5,5}, {5,4}, {5,3}});
    MockGameLogic logic(4);
    gol.setGameLogic(&logic);
    gol.step();
    EXPECT_THAT(gol.livingMembers(), ::testing::Not(::testing::Contains(addr_t({1,2}))));
}

TEST_F(GameOfLifeBoard, VivifiesDeadCellWhenItHas3Neighbors)
{
    GameOfLife gol(board_t{});
    MockGameLogic logic(3);
    logic.setWorld(board_t{{1,2}});
    gol.setGameLogic(&logic);
    gol.step();
    EXPECT_THAT(gol.livingMembers(), ::testing::ElementsAre(addr_t({1,2})));
}


TEST_F(GameOfLifeBoard, LeavesDeadCellWhenItHas2Neighbors)
{
    GameOfLife gol(board_t{});
    MockGameLogic logic(2);
    logic.setWorld(board_t{{1,2}});
    gol.setGameLogic(&logic);
    gol.step();
    EXPECT_THAT(gol.livingMembers(), ::testing::ElementsAre());
}

TEST_F(GameOfLifeBoard, EmptyBoardHasNoDeadNeighbors)
{
    GameLogic logic;
    EXPECT_THAT(logic.getDeadNeighbors(board_t{}), ::testing::ElementsAre());
}

TEST_F(GameOfLifeBoard, SingleCellHasEightDeadNeighbors)
{
    GameLogic logic;
    EXPECT_THAT(logic.getDeadNeighbors(board_t{{1,1}}), ::testing::ElementsAre(addr_t{0,0},addr_t{0,1},addr_t{0,2},addr_t{1,0},addr_t{1,2},addr_t{2,0},addr_t{2,1},addr_t{2,2}));
}

TEST_F(GameOfLifeBoard, AdjacentCellsHaveTenDeadNeighbors)
{
    GameLogic logic;
    EXPECT_THAT(logic.getDeadNeighbors(board_t{{1,1},{1,2}}), ::testing::ElementsAre(addr_t{0,0},addr_t{0,1},addr_t{0,2},addr_t{0,3},addr_t{1,0},addr_t{1,3},addr_t{2,0},addr_t{2,1},addr_t{2,2},addr_t{2,3}));
}

TEST_F(GameOfLifeBoard, ThisCellHasOneLiveNeighbor)
{
    GameLogic logic;
    EXPECT_EQ(logic.numNeighbors(addr_t{1,1}, board_t{{1,2}}), 1);
}

TEST_F(GameOfLifeBoard, CanStepArbitraryNumberOfTimes)
{
    GameOfLife gol(board_t{{1,1},{1,2},{1,0}});
    gol.step(2);
    EXPECT_THAT(gol.livingMembers(), ::testing::ElementsAre(addr_t{1,0},addr_t{1,1},addr_t{1,2}));
    gol.step(3);
    EXPECT_THAT(gol.livingMembers(), ::testing::ElementsAre(addr_t{0,1},addr_t{1,1},addr_t{2,1}));
}
